#include <assert.h>
#include <errno.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/mman.h>
#include <units.h>

#ifdef __Kernel
#include <spinel/kernelInfo.h>
#include <spinel/panic.h>
#include <spinel/virtualMemory.h>
#endif

// Simple memory allocator
// Uses buddy allocation up to 4KiB, then mmap
// Tries to stay 4-byte aligned for compatibility
// with processors that need it

// Cool fact: Spinel's mmap will, when given NULL for the address,
// place the memory mapping at the current heap's end
// So we don't need sbrk, mmap will do all the work

// Since our allocations should be 8-byte aligned
// and we use four bytes for metadata, a minimum order of 4
// (= 16 bytes) makes sense
#define BuddyMinOrder 4
// Maximum order of 12 = 4 KiB
// After this point, we will just mmap in the result
#define BuddyMaxOrder 12
// Size of the block pool to allocate at one time
// If we don't have enough blocks to meet a request,
// we will allocate another pool
#define PoolSize (16 * KiB)
// Number of max order blocks in a pool
// pow2 is an inline function defined below
#define BlocksInPool (PoolSize / pow2(BuddyMaxOrder))

typedef struct FreeMemoryBlock {
    uint8_t order;
    uint8_t isFree;
    uint8_t padding[2];
    struct FreeMemoryBlock* next;
    uint8_t data[];
} __attribute__((packed)) FreeMemoryBlock;

typedef struct AllocMemoryBlock {
    uint8_t order;
    uint8_t isFree;
    uint8_t padding[6];
    uint8_t data[];
} __attribute__((packed)) AllocMemoryBlock;

static FreeMemoryBlock* freeLists[BuddyMaxOrder - BuddyMinOrder + 1];

#ifdef __Kernel
static void* heapEnd = (void*)KernelHeapStart;
#endif

// Integer log2
static inline size_t lg(size_t x) {
    size_t res = 0;
    size_t val = x;
    while (val > 1) {
        val >>= 1;
        res++;
    }
    return res;
}

// Get a power of 2
static inline size_t pow2(size_t exp) {
    return 1 << exp;
}

static inline size_t roundToPowerOf2(size_t x) {
    // 1 is subtracted from x in case x is already a power of 2,
    // so that the function will effectively return x
    return pow2(lg(x - 1) + 1);
}

static inline FreeMemoryBlock* getBuddy(void* addr, size_t size) {
    return (FreeMemoryBlock*)((uintptr_t)addr ^ size);
}

static inline void* alignPtr(void* addr) {
    return (void*)(((uintptr_t)addr + 7) & ~7);
}

static inline void* voidToMemoryBlock(void* addr) {
    return (void*)((uintptr_t)addr - 8);
}

static inline size_t memSizeToBlockSize(size_t size) {
    return size + 8;
}

#ifdef __Kernel
// The kernel doesn't get to have syscalls, so it needs shims
static inline void* mmap(
    void* addr, size_t length, int prot, int flags, int fd, off_t offset
) {
    // We can make some reasonable assumptions:
    // addr is NULL, we're trying to map at the end of the heap (program break)
    // prot = PROT_READ | PROT_WRITE
    // flags = MAP_ANONYMOUS
    // and thus fd is -1 and offset is 0
    (void)prot;
    (void)flags;
    (void)fd;
    (void)offset;

    if (length == 0) {
        errno = EINVAL;
        return MAP_FAILED;
    }

    void* newEnd = (void*)((uintptr_t)heapEnd + length);
    newEnd = alignPtr(newEnd);
    if (newEnd >= (void*)KernelHeapEnd) {
        panic(
            "Out of heap: tried to extend heap from 0x%X to 0x%X",
            heapEnd, newEnd
        );
    } else if (newEnd < (void*)KernelHeapStart) {
        // !?!?!
        panic(
            "Out of heap: heap overflowed from 0x%X to 0x%X",
            heapEnd, newEnd
        );
    }

    int ret = vMemMapInRange(heapEnd, newEnd, VMemRead | VMemWrite);
    if (ret < 0) {
        panic("vMemMapInRange failed: %s", strerror(-ret));
    }

    void* res = heapEnd;
    heapEnd = newEnd;
    return res;
}

static inline int munmap(void* addr, size_t length) {
    if (length == 0) {
        errno = EINVAL;
        return -1;
    }

    void* end = (void*)((uintptr_t)addr + length);
    end = alignPtr(end);
    if (
        addr < (void*)KernelHeapStart || addr >= (void*)KernelHeapEnd
    ) {
        panic("Tried to shim munmap starting at invalid address 0x%X", addr);
    } else if (end < (void*)KernelHeapStart || end > heapEnd) {
        panic(
            "shim munmap of size %u at address 0x%X leaves heap into 0x%X\n"
            "(current heap end is 0x%X)",
            length, addr, end, heapEnd
        );
    }
    int ret = vMemUnmapInRange(end, heapEnd);
    if (ret < 0) {
        panic("vMemUnmapInRange failed: %s", strerror(-ret));
    }
    return 0;
}
#endif // def __Kernel

void* malloc(size_t size) {
    if (size == 0) {
        return NULL;
    }

    // Account for the order metadata
    size = memSizeToBlockSize(size);
    size_t sizeOrder = lg(roundToPowerOf2(size));
    if (sizeOrder < BuddyMinOrder) {
        sizeOrder = BuddyMinOrder;
    } else if (sizeOrder > BuddyMaxOrder) {
        // Just mmap it, then...
        size = pow2(sizeOrder);
        AllocMemoryBlock* res = mmap(
            NULL, size,
            PROT_READ | PROT_WRITE, MAP_ANONYMOUS, -1, 0
        );
        if (res == MAP_FAILED) {
            return NULL;
        }
        res->order = sizeOrder;
        return (void*)res->data;
    }

    FreeMemoryBlock* block = freeLists[sizeOrder - BuddyMinOrder];
    if (block == NULL) {
        // Oops, no blocks available! Find a new one
        for (
            size_t orderUp = sizeOrder + 1; orderUp <= BuddyMaxOrder; orderUp++
        ) {
            block = freeLists[orderUp - BuddyMinOrder];
            if (block != NULL) {
                // Awesome! Remove this from its free list
                freeLists[orderUp - BuddyMinOrder] = block->next;
                block->isFree = 0;
                break;
            }
        }

        if (block == NULL) {
            // Oops, there are no free blocks.
            // That's okay, we can make some!
            FreeMemoryBlock* newPool = mmap(
                NULL, PoolSize, PROT_READ | PROT_WRITE, MAP_ANONYMOUS, -1, 0
            );
            if (newPool == MAP_FAILED) {
                // oh goodness no we can't
                return NULL;
            }

            for (size_t i = 0; i < BlocksInPool; i++) {
                newPool->order = BuddyMaxOrder;
                newPool->isFree = 1;
                newPool->next = freeLists[BuddyMaxOrder - BuddyMinOrder];
                freeLists[BuddyMaxOrder - BuddyMinOrder] = newPool;
                newPool = (void*)newPool + pow2(BuddyMaxOrder);
            }

            // Now take a block from the free list
            block = freeLists[BuddyMaxOrder - BuddyMinOrder];
            freeLists[BuddyMaxOrder - BuddyMinOrder] = block->next;
            block->isFree = 0;
        }

        // Cool, found one!
        // Split this block up into sizeOrder blocks and allocate one
        for (
            size_t orderDown = block->order - 1;
            orderDown >= sizeOrder;
            orderDown--
        ) {
            // Split the block: we get the block and its buddy
            FreeMemoryBlock* buddy = getBuddy(block, pow2(orderDown));
            block->order = orderDown;
            buddy->order = orderDown;
            // The buddy becomes free
            buddy->isFree = 1;
            buddy->next = freeLists[orderDown - BuddyMinOrder];
            freeLists[orderDown - BuddyMinOrder] = buddy;
            // The block is further whitled down
        }
    } else {
        // Cool, there are already free blocks!
        // Dequeue the first one
        block->isFree = 0;
        freeLists[sizeOrder - BuddyMinOrder] = block->next;
    }

    // Alright, now we have an appropriately sized block,
    // fresh off the free list.
    return ((AllocMemoryBlock*)block)->data;
}

// Free is here too so it can have access to the kernel shims
void free(void* addr) {
    if (addr == NULL) {
        return;
    }

    AllocMemoryBlock* block = voidToMemoryBlock(addr);

    if (block->order > BuddyMaxOrder) {
        // This block was just mmap'd in, so just mmap it out
        int ret = munmap(block, pow2(block->order));
        return;
    }

    block->isFree = 1;
    for (
        size_t orderUp = block->order + 1;
        orderUp <= BuddyMaxOrder;
        orderUp++
    ) {
        // A bit presumptuous to call it free, but the layouts for both blocks
        // should align
        FreeMemoryBlock* buddy = getBuddy(block, pow2(block->order));
        if (!buddy->isFree) {
            // No go, the buddy is allocated
            break;
        }
        // Okay, we can keep coalescing
        // This buddy is no longer free
        buddy->isFree = 0;
        // Set this block to have its new order
        block->order = orderUp;

        // Now the buddy has been absorbed into the block
        // The issue now is yanking the buddy from the free list
        // TODO: locking
        FreeMemoryBlock* prevList = NULL;
        FreeMemoryBlock* freeList = freeLists[orderUp - 1 - BuddyMinOrder];
        // Of course, if the buddy's free, this should hold:
        assert(freeList != NULL);

        // Unfortunately, we must loop over the free list.
        for (
            ;
            freeList != NULL;
            prevList = freeList, freeList = freeList->next
        ) {
            if (freeList == buddy) {
                // Aha! There you are!
                if (prevList != NULL) {
                    // Remove the buddy from the free list
                    prevList->next = freeList->next;
                } else {
                    // This must be the first one
                    freeLists[orderUp - 1 - BuddyMinOrder] = freeList->next;
                }
                break;
            }
        }
    }
}

void* realloc(void* ptr, size_t size) {
    if (ptr == NULL) {
        // Growing nothing
        return malloc(size);
    }

    if (size == 0) {
        // Resize to 0 == freeing
        free(ptr);
        return NULL;
    }

    // Round size
    size = roundToPowerOf2(memSizeToBlockSize(size));
    AllocMemoryBlock* block = voidToMemoryBlock(ptr);
    size_t blockSize = pow2(block->order);

    // Determined by the new size or the current size, whichever is smaller
    size_t bytesToPreserve = size < blockSize ? size : blockSize;

    // Not sure if shrinking the allocated data would be good
    // This may lead to smaller fragments being created than this method
    void* res = malloc(size);
    if (res == NULL) {
        // ???
        return NULL;
    }

    memcpy(res, ptr, bytesToPreserve);
    free(ptr);
    return res;
}
