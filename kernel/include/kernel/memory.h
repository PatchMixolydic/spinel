#ifndef _KERNEL_MEMORY_H
#define _KERNEL_MEMORY_H

#include <kernel/multiboot.h>
#include <stddef.h>

extern const size_t PageSize;

void* kernelMalloc(size_t amount);
void kernelFree(void* memory);

#endif // ndef _KERNEL_MEMORY_H
