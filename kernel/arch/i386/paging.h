#ifndef ARCH_I386_PAGING_H
#define ARCH_I386_PAGING_H

#include <stdint.h>

static const uintptr_t KernelOffset = 0xC0000000;

extern void setCR3(uintptr_t* addr);
extern uintptr_t* getCR3();
// Takes a linear address. Invalidates the page in the TLB
extern void invalidatePage(void* page);

// Makes .text and .rodata read-only alongside cutting the identity mapping
void improveKernelPageStructs();
// Create page table/directory/directory pointer table/level 1 million table
uintptr_t* createPageStructure();
void deletePageStructure(uintptr_t* pageStruct);

static inline uintptr_t getPhysicalAddr(uintptr_t addr){
    return addr - KernelOffset;
}

#endif // ndef ARCH_I386_PAGING_H
