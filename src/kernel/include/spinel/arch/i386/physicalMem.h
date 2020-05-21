#ifndef PHYSICALMEM_H
#define PHYSICALMEM_H

#include <stddef.h>
#include <multiboot2.h>
#include <spinel/kernelInfo.h>

#define PageAlign(addr) ((addr) & ~0x0FFF)
// The next page, no matter what
#define NextPage(addr) (PageAlign(addr) + PageSize)
// This page if it's already page aligned, otherwise the next page
#define NearestPage(addr) \
    (((addr) & 0x0FFF) == 0 ? (addr) : NextPage(addr))

typedef multiboot_memory_map_t memmap_t;

void initPhysicalAlloc(const memmap_t* memoryMapPtr, size_t memoryMapLength);
void* allocatePageFrame(void);
void freePageFrame(void* frame);

#endif // ndef PHYSICALMEM_H
