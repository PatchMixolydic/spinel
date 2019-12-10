#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <kernel/memory.h>

const unsigned int PageSize = 4096;

// from linker script
extern const uint8_t __KernelStart[];
extern const uint8_t __KernelEnd[];

size_t kernelSize = 0;
size_t availableMemory = 0;

void initMemoryManager(multiboot_memory_map_t* memoryMap, size_t memoryMapLength) {
    kernelSize = __KernelEnd - __KernelStart;
    printf("Kernel size is %d KiB.\n", kernelSize / 1024);

    multiboot_memory_map_t* memoryMapEnd = (multiboot_memory_map_t*)(
        (uintptr_t)memoryMap + memoryMapLength
    );
	while (memoryMap < memoryMapEnd) {
		uintptr_t start = memoryMap->addr;
		uintptr_t end = memoryMap->addr + memoryMap->len - 1;
		bool valid = memoryMap->type == 1;
        if (valid) {
            availableMemory += memoryMap->len;
        }
        printf("Memory region 0x%X - 0x%X is%s available.\n", start, end, valid ? "" : "n't");
		// memoryMap->size fails to account for itself
		memoryMap = (multiboot_memory_map_t*)(
            (uintptr_t)memoryMap + memoryMap->size + sizeof(memoryMap->size)
        );
	}
    printf("%d MiB memory available.\n", availableMemory / 1024 / 1024);
}

void* allocatePageFrame() {
    return NULL;
}
