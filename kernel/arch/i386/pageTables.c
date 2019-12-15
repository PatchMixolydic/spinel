#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <kernel/memory.h>
#include "cpu.h"
#include "paging.h"

// 0: Page table
// 1: Page directory
// 2: Page directory pointer table
// 3: Page map level 4 table
// 4: Page map level 5 table
static const unsigned int MaxPageStructLevel = 1;
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

uintptr_t* currentTopPageStruct = kernelPageDirectory;

static inline size_t getTableSize() {
    return PageSize / sizeof(uintptr_t);
}

static inline size_t addrToStructIdx(uintptr_t addr, int level) {
    size_t divisor = PageSize;
    for (; level > 0; level--) {
        divisor *= getTableSize();
    }
    return ((addr) / divisor) % getTableSize();
}

static inline uintptr_t* getPageTableEntry(uintptr_t page) {
    return (uintptr_t*)(KernelPageTabOffset + (sizeof(uintptr_t) * addrToStructIdx(page, 0)));
}

static inline uintptr_t* getPageDirectoryEntry(uintptr_t page) {
    return (uintptr_t*)(KernelPageDirOffset + addrToStructIdx(page, 1) * sizeof(uintptr_t));
}

void improveKernelPageStructs() {
    kernelPageDirectory[0] = 0; // remove identity mapping
    uintptr_t* pageEntry;
    for (uintptr_t page = __TextStart; page < __TextEnd; page += PageSize) {
        // Remove read/write status
        pageEntry = getPageTableEntry(page);
        *pageEntry &= ~PageReadWriteFlag;
    }
    for (uintptr_t page = __RODataStart; page < __RODataEnd; page += PageSize) {
        pageEntry = getPageTableEntry(page);
        *pageEntry &= ~PageReadWriteFlag;
    }
    // We need somewhere to stage pages to zero them...
    uintptr_t stagePageTab = allocatePageFrame();
    uintptr_t* pageDirEntry = getPageDirectoryEntry(KernelTempPageLoc);
    *pageDirEntry = stagePageTab | PageReadWriteFlag | PagePresentFlag;
    unmapPage(KernelTempPageLoc); // erase anything that might already be here
    // map the page table
    mapPhysicalAddress(stagePageTab, KernelTempPageLoc, PageReadWriteFlag | PagePresentFlag);
    // clear it (except for this page entry -- that'd get wacky)
    memset(KernelTempPageLoc + sizeof(uintptr_t), 0, PageSize - sizeof(uintptr_t));
    unmapPage(KernelTempPageLoc);
    invalidatePage(KernelTempPageLoc);
    // Due to identity mapping being stripped and text and rodata being set
    // to readonly, we need to invalidate all pages in our table
    for (
        void* page = kernelPageTable;
        page < kernelPageTable + (getTableSize() * PageSize);
        page += PageSize
    ) {
        invalidatePage(page);
    }
}

uintptr_t* createPageStructure() {
    uintptr_t* res = (uintptr_t*)allocatePageFrame();
    mapPhysicalAddress((uintptr_t)res, KernelTempPageLoc, PageReadWriteFlag | PagePresentFlag);
    memset(KernelTempPageLoc, 0, PageSize);
    unmapPage(KernelTempPageLoc);
    return res;
}

void deletePageStructure(uintptr_t* pageStruct) {
    if (getCR3() == pageStruct) {
        printf("Freeing page structure in CR3 -- something bad is about to happen!\n");
    }
    freePageFrame(pageStruct);
}

bool mapPhysicalAddress(uintptr_t physical, uintptr_t virtual, uintptr_t flags) {
    uintptr_t* pageDirEntry = getPageDirectoryEntry(virtual);
    uintptr_t* pageTabEntry = getPageTableEntry(virtual);
    if ((*pageDirEntry & PagePresentFlag) == 0) {
        // Hey, our page table doesn't exist yet!
        uintptr_t newEntry = (uintptr_t)createPageStructure() | PageReadWriteFlag | PagePresentFlag;
        *pageDirEntry = newEntry;
        invalidatePage(virtual);
    } else if (*pageTabEntry & PagePresentFlag) {
        // Hey, this is already present! You can't have this!
        return false;
    }

    *pageTabEntry = PageAlign(physical) | PagePresentFlag | flags;
    invalidatePage(virtual);
    return true;
}

bool mapPage(uintptr_t virtual, uintptr_t flags) {
    void* frame = allocatePageFrame();
    return mapPhysicalAddress((uintptr_t)frame, virtual, flags);
}

void unmapPage(uintptr_t virtual) {
    uintptr_t* pageEntry = getPageTableEntry(virtual);
    *pageEntry = 0;
    invalidatePage(virtual);
}

void handlePageFault(Registers registers, uint32_t errorCode) {
    printf("Page fault\n");
    printf("Error code 0x%X\n", errorCode);
    printf("CR2 0x%X\n", getCR2());
}
