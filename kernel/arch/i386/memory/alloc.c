#include <stdint.h>
#include <string.h>
#include "alloc.h"
#include "paging.h"

// Current end of heap pointer
void* kernelEndOfHeap = KernelHeap;
// Current amount of memory allocated for the heap (multiple of PageSize)
void* kernelActualEndOfHeap = KernelHeap;

// TODO: This is mostly portable! This should be moved out into the
// main kernel, with allocatePageFrame replaced with a general call
// for more physical memory
void* kernelMalloc(size_t amount) {
    if (kernelEndOfHeap + amount >= KernelHeapHardLimit) {
        // That won't do!
        return NULL;
    } else if (amount == 0) {
        return kernelEndOfHeap; // :P
    }
    void* res = kernelEndOfHeap;
    while (kernelEndOfHeap + amount >= kernelActualEndOfHeap) {
        // Hmm... we need more pages
        void* frame = allocatePageFrame();
        mapPageAt(frame, kernelActualEndOfHeap, PageReadWriteFlag);
        kernelActualEndOfHeap += PageSize;
    }
    kernelEndOfHeap += amount;
    memset(res, 0, amount);
    return res;
}
