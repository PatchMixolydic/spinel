#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <kernel/memory.h>
#include <kernel/panic.h>

const size_t PageSize = 4096;

// from linker script
extern const uint8_t __KernelStart[];
extern const uint8_t __KernelEnd[];

typedef multiboot_memory_map_t memMap;

uint32_t* pageMap; // Contains info about which pages are free (1) or not (0)
uint32_t* availMap; // Contains info about which pages are available
size_t nextFreePageIdx = 0; // index of the uint32_t with the next free page
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
    return addr & ~0x0FFF;
}

// Gets the address of the first page after addr, or addr if it's a page addr
static inline uint64_t nearestPage(uint64_t addr) {
    return (addr & 0x0FFF) == 0 ? addr : (pageAlign(addr) + PageSize);
}

static inline size_t getBitmapIndex(uint64_t addr) {
    return addr / PageSize / 32; // Page index of addr divided into groups of 32
}

static inline size_t getBitmapBit(uint64_t addr) {
    return (addr / PageSize) % 32; // pageMap[0] bit 0 = 0, bit 1 = 4096...
}

static inline uintptr_t getAddr(size_t idx, size_t bit) {
    return (idx * 32 + bit) * PageSize;
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

void initPageFrameAllocator(memMap* memoryMapPtr, size_t memoryMapLength) {
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
    setPageFree((uintptr_t)NULL, false); // Zero page --  can't touch this
    setPageAvailable((uintptr_t)NULL, false);
}

void* allocatePageFrame() {
    size_t searchStartLoc = nextFreePageIdx;
    uintptr_t res = (uintptr_t)NULL;
    do {
        // pageMap[x] & availMap[x] shows if there's any pages which are free
        // and available
        if (
            pageMap[nextFreePageIdx] == 0 ||
            (pageMap[nextFreePageIdx] & availMap[nextFreePageIdx]) == 0
        ) {
            // Hey, that's not free!
            nextFreePageIdx = (nextFreePageIdx + 1) % bitmapSize;
            continue;
        }
        // Okay, there's a free page. We just need its bit.
        size_t bit;
        for (bit = 0; bit < 32; bit++) {
            uint32_t freeAndAvail = pageMap[nextFreePageIdx] & availMap[nextFreePageIdx];
            if (freeAndAvail & (1 << bit)) {
                // Cooool, this is free!
                break;
            }
        }
        if (bit == 32) {
            printf("Index %d doesn't have any free pages!\n", nextFreePageIdx);
            printf("Page map: 0x%X\n", pageMap[nextFreePageIdx]);
            printf("Available map: 0x%X\n", availMap[nextFreePageIdx]);
            panic("Sanity check failed for allocatePageFrame");
        }
        res = getAddr(nextFreePageIdx, bit);
        setPageFree(res, false);
        break;
    } while (nextFreePageIdx != searchStartLoc);
    if (res == (uintptr_t)NULL) {
        // oops, we couldn't find a page. this should swap later
        // for now, panic
        panic("Out of memory");
    }
    return (void*)res;
}

void freePageFrame(void* frame) {
    uintptr_t addr = (uintptr_t)frame;
    if (!getPageAvailable(addr)) {
        // nice try
        printf("Tried to free reserved page frame 0x%p\n", frame);
        return;
    }
    setPageFree(addr, true);
    if ((pageMap[nextFreePageIdx] & availMap[nextFreePageIdx]) == 0) {
        nextFreePageIdx = getBitmapIndex(addr);
    }
}
