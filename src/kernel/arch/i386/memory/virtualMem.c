#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <spinel/kernelInfo.h>
#include <spinel/panic.h>
#include "../core/cpu.h"
#include "../interrupts/interrupts.h"
#include "physicalMem.h"
#include "virtualMem.h"

#define PageMapSize (PageSize / sizeof(uintptr_t))

static const uint32_t PageFaultPresentFlag = 1;
static const uint32_t PageFaultWriteFlag = 1 << 1;
static const uint32_t PageFaultUserModeFlag = 1 << 2;
static const uint32_t PageFaultReservedWriteFlag = 1 << 3;
static const uint32_t PageFaultInstrFetchFlag = 1 << 4;

static const uintptr_t KernelPageMapOffset = 0xFFC00000;

// from linker script
extern const uint8_t __TextStart[];
extern const uint8_t __TextEnd[];
extern const uint8_t __RODataStart[];
extern const uint8_t __RODataEnd[];

extern uintptr_t kernelPageTable[];
extern uintptr_t kernelPageDirectory[];

typedef enum {
    PagePresentFlag = 1,
    PageReadWriteFlag = 1 << 1,
    PageUserModeFlag = 1 << 2,
    PageWriteThroughFlag = 1 << 3,
    PageCacheDisableFlag = 1 << 4,
    PageAccessedFlag = 1 << 5,
    PageDirtyFlag = 1 << 6,
    PageLargePageFlag = 1 << 7,
    PageGlobalFlag = 1 << 8
} PageFlags;

static inline size_t addrToMapIdx(uintptr_t addr, int level) {
    size_t divisor = sizeof(uintptr_t) * PageMapSize;
    for (; level > 0; level--) {
        divisor *= PageMapSize;
    }
    return ((addr) / divisor) % PageMapSize;
}

static inline uintptr_t* getPageMapEntry(uintptr_t page, size_t level) {
    // TODO
    switch (level) {
        case 0:
            return (uintptr_t*)(
                KernelPageMapOffset +
                (addrToMapIdx(page, 1) * sizeof(uintptr_t) * PageMapSize) +
                (addrToMapIdx(page, 0) * sizeof(uintptr_t))
            );
        case 1:
            return (uintptr_t*)(
                KernelPageMapOffset +
                (1023 * sizeof(uintptr_t) * PageMapSize) +
                (addrToMapIdx(page, 1) * sizeof(uintptr_t))
            );
        default:
            panic("FIXME: getPageMapEntry for a level %d pagemap!", level);
    }
    return NULL;
}

void setupPageMaps() {
    kernelPageDirectory[0] = 0; // remove identity mapping
    uintptr_t* pageEntry;
    for (
        uintptr_t page = __TextStart;
        page < (uintptr_t)__TextEnd;
        page += PageSize
    ) {
        // Remove read/write status
        pageEntry = getPageMapEntry(page, 0);
        *pageEntry &= ~PageReadWriteFlag;
    }
    for (
        uintptr_t page = __RODataStart;
        page < (uintptr_t)__RODataEnd;
        page += PageSize
    ) {
        pageEntry = getPageMapEntry(page, 0);
        *pageEntry &= ~PageReadWriteFlag;
    }

    // TODO: might be slow; we need to invalidate all the pages we just changed
    setCR3(getCR3());
}

void mapPage(uintptr_t virtual, uintptr_t flags) {
    uintptr_t* pageTabEntry = getPageMapEntry(virtual, 0);
    uintptr_t* pageDirEntry = getPageMapEntry(virtual, 1);
    if ((*pageDirEntry & PagePresentFlag) == 0) {
        // Oops, we kind of need a page table...
        uintptr_t pageTab = (uintptr_t)allocatePageFrame();
        *pageDirEntry = pageTab | PagePresentFlag | PageReadWriteFlag;
        if (virtual < KernelOffset) {
            *pageDirEntry |= PageUserModeFlag;
        }
        memset(getPageMapEntry(PageAlign(virtual), 0), 0, PageSize);
        invalidatePage((void*)virtual);
    }
    uintptr_t physical = (uintptr_t)allocatePageFrame();
    *pageTabEntry = PageAlign(physical) | PagePresentFlag | flags;
    invalidatePage((void*)virtual);
}

void unmapPage(uintptr_t virtual) {
    uintptr_t* pageEntry = getPageMapEntry(virtual, 0);
    *pageEntry = 0;
    invalidatePage((void*)virtual);
}

void handlePageFault(InterruptInfo info) {
    printf("Page fault at 0x%p\n", getCR2());
    if (KernelHeapStart <= getCR2() && getCR2() < KernelHeapEnd) {
        if (info.errorCode & PageFaultUserModeFlag) {
            printf("User process tried to allocate kernel heap memory");
            return;
        }
        // Oh, kmalloc? Nonononono, you didn't do anything wrong
        // Here's a fresh page, just for you~
        mapPage(getCR2(), PageReadWriteFlag);
        return;
    }

    panic(
        "Page fault\n"
        "CR2 0x%X    Error code 0x%X\n"
        "%s%s%s%s%s%s\n"
        "Page table entry 0x%X    Page directory entry 0x%X",
        getCR2(), info.errorCode,
        info.errorCode ? "Flags: " : "",
        info.errorCode & PageFaultPresentFlag ? "present " : "",
        info.errorCode & PageFaultWriteFlag ? "write " : "",
        info.errorCode & PageFaultUserModeFlag ? "userMode " : "",
        info.errorCode & PageFaultReservedWriteFlag ? "reservedWrite " : "",
        info.errorCode & PageFaultInstrFetchFlag ? "instructionFetch" : "",
        *getPageMapEntry(getCR2(), 0), *getPageMapEntry(getCR2(), 1)
    );
}
