#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <kernel/memory.h>

const size_t PageSize = 4096;

// from linker script
extern const uint8_t __KernelStart[];
extern const uint8_t __KernelEnd[];

typedef multiboot_memory_map_t memMap;

uint32_t* pageMap; // Contains info about which pages are allocated
uint32_t* availMap; // Contains info about which pages are available
uint32_t* earliestAllocatedPage; // Points to last used page map entry
size_t kernelReservedSize = 0;
size_t availableMemory = 0;
size_t bitmapSize = 0; // size in bytes, not length, for page and avail map

static inline memMap* nextMapEntry(memMap* ptr) {
    // memoryMapPtr->size fails to account for itself
    return (memMap*)(
        (uintptr_t)ptr + ptr->size + sizeof(ptr->size)
    );
}

// Gets the page address this address is in
static inline uint64_t pageAlign(uint64_t addr) {
    return addr & 0xFFFFFFFFFFFFF000;
}

// Gets the address of the first page after addr, or addr if it's a page addr
static inline uint64_t nearestPage(uint64_t addr) {
    return (addr & 0x0FFF) == 0 ? addr : (pageAlign(addr) + PageSize);
}

static inline size_t getBitmapIndex(uint64_t addr) {
    return addr / PageSize / 32; // Page index of addr divided into groups of 32
}

static inline uint8_t getBitmapBit(uint64_t addr) {
    return addr % 32; // pageMap[0] bit 0 = 0, bit 1 = 4096...
}

static inline void setPageAvailable(uint64_t addr, bool valid) {
    if (valid) {
        availMap[getBitmapIndex(addr)] |= 1 << getBitmapBit(addr);
    } else {
        availMap[getBitmapIndex(addr)] &= ~(1 << getBitmapBit(addr));
    }
}

static inline bool getPageAvailable(uint64_t addr) {
    return (availMap[getBitmapIndex(addr)] & (1 << getBitmapBit(addr))) != 0;
}

static inline void setPageFree(uint64_t addr, bool free) {
    if (free) {
        pageMap[getBitmapIndex(addr)] |= 1 << getBitmapBit(addr);
    } else {
        pageMap[getBitmapIndex(addr)] &= ~(1 << getBitmapBit(addr));
    }
}

static inline bool getPageFree(uint64_t addr) {
    return (pageMap[getBitmapIndex(addr)] & (1 << getBitmapBit(addr))) != 0;
}

void initMemoryManager(memMap* memoryMapPtr, size_t memoryMapLength) {
    kernelReservedSize = __KernelEnd - __KernelStart;
    printf("Kernel size is %d KiB.\n", kernelReservedSize / 1024);

    memMap* memoryMapEnd = (memMap*)(
        (uintptr_t)memoryMapPtr + memoryMapLength
    );
	for (memMap* ptr = memoryMapPtr; ptr < memoryMapEnd; ptr = nextMapEntry(ptr)) {
        availableMemory += ptr->len;
	}
    printf("%d MiB memory available.\n", availableMemory / 1024 / 1024 + 1);
    // Ok, we know how much memory there is. Now our bitmaps need to be set up.
    bitmapSize = availableMemory / PageSize;
    pageMap = __KernelEnd; // The page map is located at the end of the kernel
    memset(pageMap, 0, bitmapSize); // empty the page map
    availMap = __KernelEnd + bitmapSize; // After the page map is the availMap
    memset(availMap, 0, bitmapSize); // empty that, too
    kernelReservedSize += bitmapSize * 2; // Grow reserved size
	for (memMap* ptr = memoryMapPtr; ptr < memoryMapEnd; ptr = nextMapEntry(ptr)) {
		uint64_t start = ptr->addr;
		uint64_t end = ptr->addr + ptr->len;
		bool valid = ptr->type == 1;
        for (uint64_t addr = nearestPage(start); addr < end; addr += PageSize) {
            // If this is the kernel or the bitmap...
            if (__KernelStart <= addr && addr < __KernelEnd + kernelReservedSize) {
                setPageFree(addr, false); // These don't get to be freed or swapped out.
                setPageAvailable(addr, false);
            } else {
                // Good to go
                setPageFree(addr, valid);
                setPageAvailable(addr, valid);
            }
        }
        // TODO: no long specifiers... convert to uint in the meantime
        printf(
            "Memory region 0x%X - 0x%X is%s available.\n",
            (unsigned int)start, (unsigned int)end - 1, valid ? "" : "n't"
        );
	}
}

void* allocatePageFrame() {
    return NULL;
}
