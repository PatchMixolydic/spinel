#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <units.h>
#include <spinel/concurrency.h>
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

static const uintptr_t PageMapMemoryOffset = 0xFFC00000;
static const unsigned NumPageMapLevels = 2;
static const unsigned PageMapLength = 1024;

// from linker script
extern const uint8_t __TextStart[];
extern const uint8_t __TextEnd[];
extern const uint8_t __RODataStart[];
extern const uint8_t __RODataEnd[];

extern uintptr_t kernelPageTable[];
extern uintptr_t kernelPageDirectory[];

static Mutex virtualMemoryMutex = false;


static inline size_t addrToMapIdx(uintptr_t addr, size_t level) {
    size_t divisor = sizeof(uintptr_t) * PageMapSize;
    for (size_t i = level; i > 0; i--) {
        divisor *= PageMapSize;
    }
    return ((addr) / divisor) % PageMapSize;
}

static inline uintptr_t* getPageMapEntry(uintptr_t page, size_t level) {
    // TODO
    switch (level) {
        case 0:
            return (uintptr_t*)(
                PageMapMemoryOffset +
                (addrToMapIdx(page, 1) * sizeof(uintptr_t) * PageMapSize) +
                (addrToMapIdx(page, 0) * sizeof(uintptr_t))
            );
        case 1:
            return (uintptr_t*)(
                PageMapMemoryOffset +
                (1023 * sizeof(uintptr_t) * PageMapSize) +
                (addrToMapIdx(page, 1) * sizeof(uintptr_t))
            );
        default:
            panic("FIXME: getPageMapEntry for a level %d pagemap!", level);
    }
    return NULL;
}

static inline uintptr_t commitPage(uintptr_t addr) {
    uintptr_t physical = (uintptr_t)allocatePageFrame();
    uintptr_t* pageTabEntry = getPageMapEntry(PageAlign(addr), 0);
    // TODO: should these be soft rejections or asserts?
    assert(*pageTabEntry & PageAllocatedFlag);
    assert((*pageTabEntry & PagePresentFlag) == 0);
    uintptr_t flags = *pageTabEntry & 0xFFE;
    *pageTabEntry = PageAlign(physical) | PagePresentFlag | flags;
    invalidatePage((void*)pageTabEntry);
    invalidatePage((void*)addr);
    return physical;
}

void handlePageFault(void) {
    uintptr_t cr2 = getCR2();
    printf("Page fault at 0x%p\n", cr2);
    InterruptInfo* info = getInterruptInfo();

    uintptr_t* pageTabEntry = getPageMapEntry(cr2, 0);
    uintptr_t* pageDirEntry = getPageMapEntry(cr2, 1);

    // If the page table entry exists and the page is allocated but not present
    if (
        cr2 != (uintptr_t)NULL &&
        (*pageDirEntry & PagePresentFlag) &&
        (*pageTabEntry & PageAllocatedFlag) &&
        (*pageTabEntry & PagePresentFlag) == 0
    ) {
        // Oh, that's an easy one!
        commitPage(cr2);
        return;
    }

    panic(
        "Page fault\n"
        "CR2 0x%X    Error code 0x%X    EIP 0x%X\n"
        "%s%s%s%s%s%s\n"
        "Page table entry 0x%X    Page directory entry 0x%X",
        cr2, info->errorCode, info->eip,
        info->errorCode ? "Flags: " : "",
        info->errorCode & PageFaultPresentFlag ? "present " : "",
        info->errorCode & PageFaultWriteFlag ? "write " : "",
        info->errorCode & PageFaultUserModeFlag ? "userMode " : "",
        info->errorCode & PageFaultReservedWriteFlag ? "reservedWrite " : "",
        info->errorCode & PageFaultInstrFetchFlag ? "instructionFetch" : "",
        *getPageMapEntry(cr2, 0), *getPageMapEntry(cr2, 1)
    );
}

void initVirtualMemory(void) {
    registerInterruptHandler(IntPageFault, handlePageFault);
    spinlockMutex(&virtualMemoryMutex);
    // remove identity mapping
    kernelPageDirectory[0] = (uintptr_t)NULL;

    uintptr_t* pageEntry;
    for (
        uintptr_t page = (uintptr_t)__TextStart;
        page < (uintptr_t)__TextEnd;
        page += PageSize
    ) {
        // Mark unwritable
        pageEntry = getPageMapEntry(page, 0);
        *pageEntry &= ~PageWritableFlag;
    }

    for (
        uintptr_t page = (uintptr_t)__RODataStart;
        page < (uintptr_t)__RODataEnd;
        page += PageSize
    ) {
        pageEntry = getPageMapEntry(page, 0);
        *pageEntry &= ~PageWritableFlag;
    }

    // Map in the heap's pages
    mapPageRange(KernelHeapStart, KernelHeapEnd, PageWritableFlag);

    // TODO: might be slow; we need to invalidate all the pages we just changed
    setCR3(getCR3());
    unlockMutex(&virtualMemoryMutex);
}

void mapPage(uintptr_t virtual, uintptr_t flags) {
    spinlockMutex(&virtualMemoryMutex);
    uintptr_t* pageTabEntry = getPageMapEntry(virtual, 0);
    uintptr_t* pageDirEntry = getPageMapEntry(virtual, 1);

    if ((*pageDirEntry & PagePresentFlag) == 0) {
        // Oops, we kind of need a page table...
        uintptr_t pageTab = (uintptr_t)allocatePageFrame();
        *pageDirEntry = pageTab | PagePresentFlag | PageWritableFlag;
        if (flags & PageUserModeFlag) {
            *pageDirEntry |= PageUserModeFlag;
        }
        memset(getPageMapEntry(PageAlign(virtual), 0), 0, PageSize);
        invalidatePage((void*)pageDirEntry);
    }

    // Your memory's ready for pickup!*
    // * no it's not
    flags |= PageAllocatedFlag;
    // Sanity check: these flags should only affect bits 11-1
    // No setting the present flag or address!
    *pageTabEntry = flags & 0xFFE;
    invalidatePage((void*)pageTabEntry);
    invalidatePage((void*)virtual);
    unlockMutex(&virtualMemoryMutex);
}

void mapPageRange(uintptr_t start, uintptr_t end, uintptr_t flags) {
    for (
        uintptr_t page = PageAlign(start);
        page < NearestPage(end);
        page += PageSize
    ) {
        mapPage(page, flags);
    }
}

void unmapPage(uintptr_t virtual) {
    spinlockMutex(&virtualMemoryMutex);
    uintptr_t* pageEntry = getPageMapEntry(virtual, 0);
    uintptr_t physAddr = PageAlign(*pageEntry);
    freePageFrame((void*)physAddr);
    *pageEntry = 0;
    invalidatePage((void*)pageEntry);
    invalidatePage((void*)virtual);
    unlockMutex(&virtualMemoryMutex);
}
