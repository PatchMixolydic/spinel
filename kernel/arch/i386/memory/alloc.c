#include <stdint.h>
#include "paging.h"

static const uint32_t AllocatedMagic = 0xA110C47D;
static const uint32_t FreeMagic = 0x33DEAD33;
static const uint32_t SkippedOverMagic = 0x0CA7F00D;
static const uintptr_t TempPageLocation = 0xF0000000;
static const uintptr_t FirstPage = 0x1000;

typedef struct MemoryHeader {
    uint32_t magicNum;
    size_t size;
    struct MemoryHeader* prev;
    struct MemoryHeader* next;
} __attribute__((packed)) MemoryHeader;

void* allocateMemory(size_t amount) {
    // First, we need a free block...
    MemoryHeader* block = FirstPage;
    mapPage(FirstPage, PageReadWriteFlag); // This will only succeed if there's no page already
    if (block->magicNum != AllocatedMagic && block->magicNum != FreeMagic) {
        // We don't have a header yet -- make one
        block->magicNum = FreeMagic;
        block->size = PageSize - sizeof(MemoryHeader);
        block->prev = NULL;
        block->next = NULL;
    } else {
        while (block->magicNum != FreeMagic && block->next != NULL) {
            block = block->next;
        }
        if (block->magicNum != FreeMagic) {
            // Barnacles -- time to allocate a new page
            MemoryHeader* nextBlock = NextPage((uintptr_t)block);
            mapPage(nextBlock, PageReadWriteFlag);
            block->next = nextBlock;
            nextBlock->magicNum = FreeMagic;
            nextBlock->size = PageSize - sizeof(MemoryHeader);
            nextBlock->prev = block;
            block = nextBlock;
        }
    }
    // Let's make sure we have a big enough block...
    while (block->magicNum == AllocatedMagic || block->size > amount) {
        // We don't! We need more space...
        if (block->next != NULL) {
            block = block->next;
        } else {
            // Oops -- no blocks are big enough!
            MemoryHeader* nextBlock = NextPage((uintptr_t)block);
            mapPage(nextBlock, PageReadWriteFlag);
            if (block->magicNum == AllocatedMagic) {
                block->next = nextBlock;
                nextBlock->magicNum = FreeMagic;
                nextBlock->size = PageSize - sizeof(MemoryHeader);
                nextBlock->prev = block;
                block = nextBlock;
            } else {
                block->size += PageSize;
            }
        }
    }
    // Excellent, now we can allocate our memory!
    block->magicNum = AllocatedMagic;
    if (block->size != amount) {
        if (block->next != NULL) {
            if (block->next->magicNum == FreeMagic) {
                MemoryHeader* nextBlock = block + amount;
                nextBlock->magicNum = FreeMagic;
                nextBlock->size = block->next->size + amount;
                nextBlock->prev = block;
                nextBlock->next = block->next->next;
                block->next = nextBlock;
                block->size = amount;
            } else if (block->size - amount >= sizeof(MemoryHeader)) {
                // We have enough room to store the next header
                MemoryHeader* nextBlock = block + amount;
                nextBlock->magicNum = FreeMagic;
                nextBlock->size = block->size - amount - sizeof(MemoryHeader);
                nextBlock->prev = block;
                nextBlock->next = block->next;
                block->next->magicNum = SkippedOverMagic;
                block->next = nextBlock;
                block->size = amount;
            } else {
                // Next free space isn't big enough for the header
                // Just grab it here -- block size stays the same
            }
        } else {
            if (block->size - amount >= sizeof(MemoryHeader)) {
                MemoryHeader* nextBlock = block + amount;
                block->next = nextBlock;
                nextBlock->magicNum = FreeMagic;
                nextBlock->size = block->size - amount - sizeof(MemoryHeader);
                nextBlock->prev = NULL;
                nextBlock->next = NULL;
                block->size = amount;
            }
        }
    }
    // Awesome! Here's your new memory region
    void* res = (void*)block + sizeof(MemoryHeader);
    return res;
}

void freeMemory(void* memory) {

}
