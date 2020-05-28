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

// Simpler memory allocator

// Size of the pool to allocate at one time
// If we don't have enough free space to meet a request,
// we will allocate another pool
#define PoolSize (16 * KiB)
#define MinBlockSize (16 + sizeof(MemoryBlock)) // bytes
#define Min(x,y) ((x) < (y) ? (x) : (y))
#define Max(x,y) ((x) > (y) ? (x) : (y))

typedef struct MemoryBlock {
    uint64_t size : 63;
    uint64_t isFree : 1;
    struct MemoryBlock* nextFree;
    struct MemoryBlock* nextBlock;
    struct MemoryBlock* prevBlock;
    uint8_t data[];
} __attribute__((packed)) MemoryBlock;

static MemoryBlock* freeList = NULL;
static MemoryBlock* largestBlockAddress = NULL;

#ifdef __Kernel
static void* heapEnd = (void*)KernelHeapStart;
#endif

static inline void blockSetSize(MemoryBlock* block, uint64_t size) {
    assert(block != NULL);
    block->size = (size & ~1) >> 1;
}

static inline uint64_t blockGetSize(MemoryBlock* block) {
    assert(block != NULL);
    return block->size << 1;
}

// Literal next block, not next free block
static inline MemoryBlock* blockGetNextBlock(MemoryBlock* block) {
    assert(block != NULL);
    return (MemoryBlock*)((uintptr_t)block + (uintptr_t)blockGetSize(block));
}

static inline void blockMergeWithNext(MemoryBlock* block) {
        assert(block != NULL);
        assert(block->nextBlock != NULL && block->nextBlock->isFree);

        size_t newSize = blockGetSize(block) + blockGetSize(block->nextBlock);
        block->nextBlock = block->nextBlock->nextBlock;
        if (block->nextBlock) {
            block->nextBlock->prevBlock = block;
        }
        blockSetSize(block, newSize);
}

static inline void freeListAdd(MemoryBlock* block) {
    assert(block != NULL);
    assert(block->isFree);
    block->nextFree = freeList;
    freeList = block;
}

static inline void* alignPtr(void* addr) {
    return (void*)(((uintptr_t)addr + 7) & ~7);
}

static inline MemoryBlock* voidToMemoryBlock(void* addr) {
    return (MemoryBlock*)((uintptr_t)addr - sizeof(MemoryBlock));
}

static inline size_t memSizeToBlockSize(size_t size) {
    size_t res = size + sizeof(MemoryBlock);
    // Must be even for 63-bit size value
    if (res % 2 == 1) {
        ++res;
    }
    return res;
}

#ifdef __Kernel
// The kernel doesn't get to have syscalls, so it needs a shim
static inline void* sbrk(intptr_t amt) {
    void* oldHeap = heapEnd;
    heapEnd += amt;
    vMemMapInRange(oldHeap, heapEnd, VMemRead | VMemWrite);
    if (heapEnd < (void*)KernelHeapStart || heapEnd > (void*)KernelHeapEnd) {
        panic(
            "shim sbrk(%u) changed heap from 0x%X into 0x%X\n",
            amt, oldHeap, heapEnd
        );
    }
    return oldHeap;
}
#endif // def __Kernel

void* malloc(size_t size) {
    if (size == 0) {
        return NULL;
    }

    size = memSizeToBlockSize(size);
    MemoryBlock* block = NULL;
    if (freeList != NULL) {
        MemoryBlock* prevBlock = NULL;
        MemoryBlock* curBlock = freeList;
        for (
            ;
            curBlock != NULL;
            prevBlock = curBlock, curBlock = curBlock->nextFree
        ) {
            if (blockGetSize(curBlock) >= size) {
                // Cool, this one fits!
                if (prevBlock != NULL) {
                    // Remove from linked list
                    prevBlock->nextFree = curBlock->nextFree;
                } else {
                    // First item in list
                    freeList = curBlock->nextFree;
                }
                block = curBlock;
                block->isFree = 0;
                break;
            }
        }
    }

    if (block == NULL) {
        // couldn't find a block big enough, make one
        size_t poolSize = Max(memSizeToBlockSize(PoolSize), size);
        block = sbrk(poolSize);
        if (block == MAP_FAILED) {
            // oh stars
            return NULL;
        }
        memset(block, 0, sizeof(MemoryBlock));
        blockSetSize(block, poolSize);
        block->prevBlock = largestBlockAddress;
        if (block->prevBlock) {
            block->prevBlock->nextBlock = block;
        }
        largestBlockAddress = block;
        if (block->prevBlock) assert(block->prevBlock->nextBlock == block);
    }

    // To drop blockSize when not needed
    {
        size_t blockSize = blockGetSize(block);
        if (size < blockSize && blockSize - size >= MinBlockSize) {
            // Split this block
            blockSetSize(block, size);
            MemoryBlock* nextBlock = (MemoryBlock*)((uintptr_t)block + size);
            blockSetSize(nextBlock, blockSize - size);
            nextBlock->isFree = 1;
            nextBlock->prevBlock = block;
            nextBlock->nextBlock = block->nextBlock;
            block->nextBlock = nextBlock;
            if (nextBlock->nextBlock != NULL) {
                // Set the block after nextBlock to point back to
                // nextBlock
                nextBlock->nextBlock->prevBlock = nextBlock;
            } else {
                // This should be the block with the highest address
                if (nextBlock > largestBlockAddress) {
                    largestBlockAddress = nextBlock;
                }
            }
            freeListAdd(nextBlock);
        }
    }

    if (block->prevBlock) {
        // TODO: there's a hole in this logic somewhere
        // such that prevBlock->nextBlock isn't being set.
        // This is a hackfix.
        block->prevBlock->nextBlock = block;
    }

    // And there we have it
    return block->data;
}

// Free is here too so it can have access to the kernel shim
void free(void* addr) {
    if (addr == NULL) {
        return;
    }

    MemoryBlock* block = voidToMemoryBlock(addr);
    assert(!block->isFree);
    // Cool, here's our block!
    // Check for coalescence
    bool alreadyFree = false;
    if (block->prevBlock != NULL && block->prevBlock->isFree) {
        // Combine block into previous block
        assert(block->prevBlock->nextBlock == block);
        block->isFree = 1;
        block = block->prevBlock;
        blockMergeWithNext(block);
        // The conditions for merging imply this block is on the free list
        alreadyFree = true;
    }

    if (block->nextBlock != NULL && block->nextBlock->isFree) {
        blockMergeWithNext(block);
    }

    if (!alreadyFree) {
        block->isFree = 1;
        freeListAdd(block);
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

    MemoryBlock* block = voidToMemoryBlock(ptr);
    size_t oldAllocSize = blockGetSize(block) - sizeof(MemoryBlock);

    // Determined by the new size or the current size, whichever is smaller
    size_t bytesToPreserve = Min(size, oldAllocSize);

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
