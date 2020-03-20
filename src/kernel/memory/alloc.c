#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <spinel/alloc.h>
#include <spinel/concurrency.h>
#include <spinel/kernelInfo.h>
#include <spinel/panic.h>

typedef enum {
    Free = 0x3333,
    Allocated = 0xA110,
    Dead = 0xDEAD
} HeapStatus;

typedef struct HeapData {
    HeapStatus status;
    struct HeapData* prev;
    struct HeapData* next;
    size_t size;
} HeapData;

static bool allocInitialized = false;
static size_t heapSize;
static Mutex heapDataMutex;

static HeapData* firstFreeData;

// We're the kernel! We own the entire kernel heap. Cool!
// If kmalloc faults, the VMM will give kmalloc a page because she is cute

static HeapData* getHeapData(void* ptr) {
    return (HeapData*)((uintptr_t)ptr - sizeof(HeapData));
}

void initAlloc(void) {
    heapSize = KernelHeapEnd - KernelHeapStart;
    firstFreeData = (HeapData*)KernelHeapStart;
    *firstFreeData = (HeapData){
        Free,
        NULL,
        NULL,
        heapSize - sizeof(HeapData)
    };
    allocInitialized = true;
    printf("kmalloc initiated.\n");
}

void* kmalloc(size_t size) {
    assert(allocInitialized);
    assert(firstFreeData != NULL);
    HeapData* free = firstFreeData;

    // Go until either we find a big enough block or free->next is NULL
    spinlockMutex(&heapDataMutex);
    for (
        ;
        free->size < size && free->status != Free && free->next != NULL;
        free = free->next
    ) {}

    if (free->size < size || free->status != Free) {
        // there are no blocks big enough :<
        panic("kmalloc failed (size %d bytes)", size);
    }

    // Time to allocate
    free->status = Allocated;

    // Check if big enough to hold new heapdata + useful storage
    // ...
    // ...headpata.
    if (free->size > size + sizeof(HeapData) + 16) {
        // Alrighty! We can split this block up
        size_t newSize = free->size - size - sizeof(HeapData);
        free->size = size;
        // skip this HeapData and the allocated size
        HeapData* newData =
            (HeapData*)((uintptr_t)free + sizeof(HeapData) + size);
        *newData = (HeapData) {
            Free, // status
            free, // previous
            free->next,
            newSize
        };
        free->next = newData;
    }

    if (firstFreeData == free) {
        // Find the next one
        while (firstFreeData->status != Free && firstFreeData->next != NULL) {
            firstFreeData = firstFreeData->next;
        }
        if (firstFreeData->status != Free) {
            // Uh...
            while (
                firstFreeData->status != Free &&
                firstFreeData->prev != NULL
            ) {
                firstFreeData = firstFreeData->prev;
            }
            if (firstFreeData->status != Free) {
                // uh oh, the heap is full.
                printf(
                    "Warning: kernel heap is full. "
                    "Setting firstFreeData to NULL.\n"
                );
                firstFreeData = NULL;
            }
        }
    }
    unlockMutex(&heapDataMutex);

    return (void*)((uintptr_t)free + sizeof(HeapData));
}

void* krealloc(void* ptr, size_t size) {
    assert(allocInitialized);

    if (ptr == NULL) {
        // Growing nothing
        return kmalloc(size);
    }

    if (size == 0) {
        // Resize to 0 == freeing
        kfree(ptr);
        return NULL;
    }

    spinlockMutex(&heapDataMutex);
    HeapData* data = getHeapData(ptr);

    // Determined by the new size or the current size, whichever is smaller
    size_t bytesToPreserve = size < data->size ? size : data->size;
    unlockMutex(&heapDataMutex);

    // Not sure if shrinking the allocated data would be good
    // This may lead to smaller fragments being created than this method
    void* res = kmalloc(size);
    if (res == NULL) {
        // ???
        return NULL;
    }

    memcpy(res, ptr, bytesToPreserve);
    kfree(ptr);
    return res;
}

void kfree(void* alloc) {
    assert(allocInitialized);
    if (alloc == NULL) {
        return;
    }

    spinlockMutex(&heapDataMutex);
    HeapData* data = getHeapData(alloc);
    data->status = Free;
    if (data->prev != NULL && data->prev->status == Free) {
        // Combine
        data->prev->size += data->size;
        data->prev->next = data->next;
        data->status = Dead;
        data = data->prev;
        // The old data just becomes random noise in an uninitialized heap
        // No need to reclaim it -- it will be overwritten in due time
    }
    if (data->next != NULL && data->next->status == Free) {
        // Combine
        data->size += data->next->size;
        data->next->status = Dead;
        data->next = data->next->next;
    }
    if (firstFreeData == NULL || (uintptr_t)data < (uintptr_t)firstFreeData) {
        firstFreeData = data;
    }
    unlockMutex(&heapDataMutex);
}
