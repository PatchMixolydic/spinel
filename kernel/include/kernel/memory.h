#ifndef _KERNEL_MEMORY_H
#define _KERNEL_MEMORY_H

#include <kernel/multiboot.h>
#include <stddef.h>

extern const unsigned int PageSize;

void initMemoryManager(multiboot_memory_map_t* memoryMap, size_t memoryMapLength);
void* allocatePageFrame();

#endif // ndef _KERNEL_MEMORY_H
