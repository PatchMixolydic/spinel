#ifndef ARCH_I386_PAGING_H
#define ARCH_I386_PAGING_H

#include <stdbool.h>
#include <stdint.h>
#include <kernel/memory.h>
#include "cpu.h"

// This isn't C++, so this is what we get for generics
#define PageAlign(addr) (addr & ~0x0FFF)
// This page if it's already page aligned, otherwise the next page
#define NearestPage(addr) ((addr & 0x0FFF) == 0 ? addr : (PageAlign(addr) + PageSize))
// The next page, no matter what
#define NextPage(addr) (PageAlign(addr) + PageSize)

static const uintptr_t KernelOffset = 0xC0000000;
static const uintptr_t KernelTempPageLoc = 0xF0000000;
static const uintptr_t KernelPageDirOffset = 0xFFFFF000;
static const uintptr_t KernelPageTabOffset = 0xFFF00000;

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

extern void setCR3(uintptr_t* addr);
extern uintptr_t* getCR3();
extern uintptr_t* getCR2();
// Takes a linear address. Invalidates the page in the TLB
extern void invalidatePage(void* page);

// Makes .text and .rodata read-only alongside cutting the identity mapping
void improveKernelPageStructs();
// Create page table/directory/directory pointer table/level 1 million table
uintptr_t* createPageStructure();
void deletePageStructure(uintptr_t* pageStruct);
bool mapPhysicalAddress(uintptr_t physical, uintptr_t virtual, uintptr_t flags);
bool mapPage(uintptr_t virtual, uintptr_t flags);
void unmapPage(uintptr_t virtual);
void handlePageFault(Registers registers, uint32_t errorCode);

static inline uintptr_t getPhysicalAddr(uintptr_t addr){
    return addr - KernelOffset;
}

#endif // ndef ARCH_I386_PAGING_H
