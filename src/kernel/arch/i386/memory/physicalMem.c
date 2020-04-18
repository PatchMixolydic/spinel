#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <spinel/concurrency.h>
#include <spinel/kernelInfo.h>
#include <spinel/panic.h>
#include "physicalMem.h"

// from linker script
extern const uint8_t __KernelStart[];
extern const uint8_t __KernelEnd[];

static uint32_t* pageMap; // Contains info about which pages are free (1) or not (0)
static uint32_t* availMap; // Contains info about which pages are available
static size_t nextFreePageIdx = 0; // index of the uint32_t with the next free page
static size_t availableMemory = 0;
static size_t bitmapSize = 0; // size in bytes, not length, for page and avail map
static Mutex pageFrameMutex = false;

static inline size_t getBitmapIndex(uint64_t addr) {
    return addr / PageSize / 32; // Page idx of addr divided into groups of 32
}

static inline size_t getBitmapBit(uint64_t addr) {
    return (addr / PageSize) % 32; // pageMap[0] bit 0 = 0, bit 1 = 4096...
}

static inline uintptr_t getAddr(size_t idx, size_t bit) {
    return (idx * 32 + bit) * PageSize;
}

static inline void setPageAvailable(uint64_t addr, bool available) {
    if (available) {
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

void initPhysicalAlloc(const memmap_t* memoryMapPtr, size_t memoryMapLength) {
    size_t kernelReservedSize = __KernelEnd - __KernelStart;
    printf("Kernel size is %d KiB.\n", kernelReservedSize / 1024);

    memmap_t* memoryMapEnd = (memmap_t*)(
        (uintptr_t)memoryMapPtr + memoryMapLength
    );
    for (
        memmap_t* ptr = (memmap_t*)memoryMapPtr;
        ptr < memoryMapEnd;
        ptr++
    ) {
        availableMemory += ptr->len;
    }
    printf("%d MiB memory available.\n", availableMemory / 1024 / 1024 + 1);

    // Ok, we know how much memory there is. Now our bitmaps need to be set up.
    bitmapSize = availableMemory / PageSize;
    // The page map is located at the end of the kernel
    pageMap = (uint32_t*)__KernelEnd;
    // After the page map is the availMap
    availMap = (uint32_t*)(__KernelEnd + bitmapSize);
    kernelReservedSize += bitmapSize * 2; // Grow reserved size

    for (
        memmap_t* ptr = (memmap_t*)memoryMapPtr;
        ptr < memoryMapEnd;
        ptr++
    ) {
        uint64_t start = ptr->addr;
        if (start = 0xFFFFFFFFFFFFFFFF) {
            // Weird hackfix, this isn't a valid pointer
            break;
        }
        uint64_t end = ptr->addr + ptr->len;
        bool valid = ptr->type == MULTIBOOT_MEMORY_AVAILABLE;
        for (
            uint64_t addr = NearestPage(start);
            addr < end;
            addr += PageSize
        ) {
            // If this is the kernel or the bitmap...
            if (
                (uintptr_t)__KernelStart <= addr && \
                addr < (uintptr_t)__KernelStart + kernelReservedSize
            ) {
                // These don't get to be freed or swapped out.
                setPageFree(addr, false);
                setPageAvailable(addr, false);
            } else {
                // Good to go
                setPageFree(addr, valid);
                setPageAvailable(addr, valid);
            }
        }
    }
    setPageFree((uintptr_t)NULL, false); // Zero page --  can't touch this
    setPageAvailable((uintptr_t)NULL, false);
}

void* allocatePageFrame(void) {
    spinlockMutex(&pageFrameMutex);
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
        size_t bit = 0;
        for (; bit < 32; bit++) {
            uint32_t freeAndAvail =
                pageMap[nextFreePageIdx] & availMap[nextFreePageIdx];
            if (freeAndAvail & (1 << bit)) {
                // Cooool, this is free!
                break;
            }
        }
        if (bit == 32) {
            panic(
                "Index %d has no free pages.\n"
                "Sanity check failed for allocatePageFrame.\n"
                "Page map entry: 0x%X    Available map entry: 0x%X",
                nextFreePageIdx,
                pageMap[nextFreePageIdx],
                availMap[nextFreePageIdx]
            );
        }
        res = getAddr(nextFreePageIdx, bit);
        setPageFree(res, false);
        break;
    } while (nextFreePageIdx != searchStartLoc);
    if (res == 0) {
        // Oops! We don't have enough memory! We should try swapping later
        // For now, panic
        panic("Out of memory");
    }
    unlockMutex(&pageFrameMutex);
    return (void*)res;
}

void freePageFrame(void* frame) {
    uintptr_t addr = (uintptr_t)frame;
    if (!getPageAvailable(addr)) {
        // nice try
        printf("Tried to free reserved page frame 0x%p\n", frame);
        return;
    }
    spinlockMutex(&pageFrameMutex);
    setPageFree(addr, true);
    if ((pageMap[nextFreePageIdx] & availMap[nextFreePageIdx]) == 0) {
        nextFreePageIdx = getBitmapIndex(addr);
    }
    unlockMutex(&pageFrameMutex);
}
