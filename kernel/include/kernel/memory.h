#ifndef _KERNEL_MEMORY_H
#define _KERNEL_MEMORY_H

#include <kernel/multiboot.h>
#include <stddef.h>

extern const size_t PageSize;

void initPageFrameAllocator(multiboot_memory_map_t* memoryMap, size_t memoryMapLength);
void* allocatePageFrame();
void freePageFrame(void* frame);

#endif // ndef _KERNEL_MEMORY_H
