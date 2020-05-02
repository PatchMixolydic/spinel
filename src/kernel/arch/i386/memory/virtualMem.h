#ifndef VIRTUALMEM_H
#define VIRTUALMEM_H

#include "../interrupts/interrupts.h"

typedef enum {
    PagePresentFlag = 1,
    PageWritableFlag = 1 << 1,
    PageUserModeFlag = 1 << 2,
    PageWriteThroughFlag = 1 << 3,
    PageCacheDisableFlag = 1 << 4,
    PageAccessedFlag = 1 << 5,
    PageDirtyFlag = 1 << 6,
    PageLargePageFlag = 1 << 7,
    PageGlobalFlag = 1 << 8,
    // The meaning of this flag is implementation defined,
    // The CPU doesn't use it! We can use it for marking pages as allocated
    PageAllocatedFlag = 1 << 9,
} PageFlags;

void initVirtualMemory(void);
// Map a page into the page table
// Note that this may not allocate the backing page frame,
// but the page fault handler should handle that
void mapPage(uintptr_t virtual, uintptr_t flags);
// Map a range of pages from start to end
// (inclusive of start, exclusive of end, unless end is not page aligned)
void mapPageRange(uintptr_t start, uintptr_t end, uintptr_t flags);
void unmapPage(uintptr_t virtual);

#endif // ndef VIRTUALMEM_H
