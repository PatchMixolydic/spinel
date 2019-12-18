#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <kernel/memory.h>
#include "../cpu.h"
#include "alloc.h"
#include "paging.h"

#define TableSize (PageSize / sizeof(uintptr_t))

// 0: Page table
// 1: Page directory
// 2: Page directory pointer table
// 3: Page map level 4 table
// 4: Page map level 5 table
static const unsigned int MaxPageMapLevel = 1;

static const uint32_t PageFaultPresentFlag = 1;
static const uint32_t PageFaultWriteFlag = 1 << 1;
static const uint32_t PageFaultUserModeFlag = 1 << 2;
static const uint32_t PageFaultReservedWriteFlag = 1 << 3;
static const uint32_t PageFaultInstrFetchFlag = 1 << 4;

// from linker script
extern const uint8_t __TextStart[];
extern const uint8_t __TextEnd[];
extern const uint8_t __RODataStart[];
extern const uint8_t __RODataEnd[];

extern uintptr_t kernelPageTable[];
extern uintptr_t kernelPageDirectory[];

uintptr_t* currentTopPageMap = kernelPageDirectory;

static inline size_t addrToMapIdx(uintptr_t addr, int level) {
    size_t divisor = sizeof(uintptr_t) * TableSize;
    for (; level > 0; level--) {
        divisor *= TableSize;
    }
    return ((addr) / divisor) % TableSize;
}

static inline uintptr_t* getPageMapEntry(uintptr_t page, size_t level) {
    // TODO
    switch (level) {
        case 0:
            return KernelPageMapOffset + (addrToMapIdx(page, 1) * sizeof(uintptr_t) * TableSize) + (addrToMapIdx(page, 0) * sizeof(uintptr_t));
        case 1:
            return KernelPageMapOffset + (1023 * sizeof(uintptr_t) * TableSize) + (addrToMapIdx(page, 1) * sizeof(uintptr_t));
        default:
            panic("FIXME: getPageMapEntry for a level %d pagemap!", level);
    }
}

void improveKernelPageStructs() {
    kernelPageDirectory[0] = 0; // remove identity mapping
    uintptr_t* pageEntry;
    for (uintptr_t page = __TextStart; page < __TextEnd; page += PageSize) {
        // Remove read/write status
        pageEntry = getPageMapEntry(page, 0);
        *pageEntry &= ~PageReadWriteFlag;
    }
    for (uintptr_t page = __RODataStart; page < __RODataEnd; page += PageSize) {
        pageEntry = getPageMapEntry(page, 0);
        *pageEntry &= ~PageReadWriteFlag;
    }

    // Due to identity mapping being stripped and text and rodata being set
    // to readonly, we need to invalidate all pages in our table
    // Eventually, we will have to invalidate more tables as the rest of the
    // kernel will get mapped in
    // This is probably faster in the long run... Even if it isn't, we only
    // do this once, so it should be negligible...
    setCR3(getCR3());
}

void mapPageAt(uintptr_t physical, uintptr_t virtual, uintptr_t flags) {
    uintptr_t* pageTabEntry = getPageMapEntry(virtual, 0);
    uintptr_t* pageDirEntry = getPageMapEntry(virtual, 1);
    if ((*pageDirEntry & PagePresentFlag) == 0) {
        // Oops, we kind of need a page table...
        uintptr_t frame = (uintptr_t)allocatePageFrame();
        *pageDirEntry = frame | PagePresentFlag | PageReadWriteFlag;
        if (virtual < KernelOffset) {
            *pageDirEntry |= PageUserModeFlag;
        }
        memset(getPageMapEntry(PageAlign(virtual), 0), 0, PageSize);
    }
    *pageTabEntry = PageAlign(physical) | PagePresentFlag | flags;
    invalidatePage(virtual);
}

void unmapPage(uintptr_t virtual) {
    uintptr_t* pageEntry = getPageMapEntry(virtual, 0);
    *pageEntry = 0;
    invalidatePage(virtual);
}

void handlePageFault(Registers regs, unsigned int errorCode) {
    panic(
        "Page fault\nCR2 0x%x\t\tError code 0x%X\n%s%s%s%s%s%s",
        getCR2(), errorCode,
        errorCode ? "Flags: " : "",
        errorCode & PageFaultPresentFlag ? "present " : "",
        errorCode & PageFaultWriteFlag ? "write " : "",
        errorCode & PageFaultUserModeFlag ? "userMode " : "",
        errorCode & PageFaultReservedWriteFlag ? "reservedWrite " : "",
        errorCode & PageFaultInstrFetchFlag ? "instructionFetch" : ""
    );
}
