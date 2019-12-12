#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <kernel/memory.h>
#include "paging.h"

static const uintptr_t PresentFlag = 1;
static const uintptr_t ReadWriteFlag = 1 << 1;
static const uintptr_t UserModeFlag = 1 << 2;
static const uintptr_t WriteThroughFlag = 1 << 3;
static const uintptr_t CacheDisableFlag = 1 << 4;
static const uintptr_t AccessedFlag = 1 << 5;
static const uintptr_t DirtyFlag = 1 << 6;
static const uintptr_t LargePageFlag = 1 << 7;
static const uintptr_t GlobalFlag = 1 << 8;

// from linker script
extern const uint8_t __TextStart[];
extern const uint8_t __TextEnd[];
extern const uint8_t __RODataStart[];
extern const uint8_t __RODataEnd[];

extern uintptr_t kernelPageTable[];
extern uintptr_t kernelPageDirectory[];

static inline size_t getTableSize() {
    return PageSize / sizeof(uintptr_t);
}

static inline size_t physAddrToTableIdx(uintptr_t addr) {
    return ((addr) / PageSize) % getTableSize();
}

void improveKernelPageStructs() {
    kernelPageDirectory[0] = 0; // remove identity mapping
    for (uintptr_t page = __TextStart; page < __TextEnd; page += PageSize) {
        // Remove read/write status
        kernelPageTable[physAddrToTableIdx(getPhysicalAddr(page))] &= ~ReadWriteFlag;
    }
    for (uintptr_t page = __RODataStart; page < __RODataEnd; page += PageSize) {
        kernelPageTable[physAddrToTableIdx(getPhysicalAddr(page))] &= ~ReadWriteFlag;
    }
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
    memset(res, 0, getTableSize());
    return res;
}

void deletePageStructure(uintptr_t* pageStruct) {
    if (getCR3() == pageStruct) {
        printf("Freeing page structure in CR3 -- something bad is about to happen!\n");
    }
    freePageFrame(pageStruct);
}
