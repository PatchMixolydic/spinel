#ifndef ARCH_I386_PAGING_H
#define ARCH_I386_PAGING_H

#include <stdint.h>

static const uintptr_t KernelOffset = 0xC0000000;

extern void setCR3(uintptr_t* addr);
extern uintptr_t* getCR3();

void setupPaging();
// Create page table/directory/directory pointer table/level 1 million table
uintptr_t* createPageStructure();

#endif // ndef ARCH_I386_PAGING_H
