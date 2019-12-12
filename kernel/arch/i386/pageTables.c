#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <kernel/memory.h>
#include "paging.h"

static const uintptr_t PresentFlag = 1;
static const uintptr_t ReadWriteFlag = 1 << 1;
static const uintptr_t UserModeFlag = 1 << 2;
static const uintptr_t WriteThroughFlag = 1 << 3;
static const uintptr_t CacheDisableFlag = 1 << 4;
static const uintptr_t AccessedFlag = 1 << 5;
static const uintptr_t DirtyFlag = 1 << 6;
static const uintptr_t LargePageFlag = 1 << 7;
static const uintptr_t GlobalFlag = 1 << 8;

static inline size_t getTableSize() {
    return PageSize / sizeof(uintptr_t);
}

uintptr_t* createPageStructure() {
    uintptr_t* res = (uintptr_t*)allocatePageFrame();
    
    for (int i = 0; i < getTableSize(); i++) {
        res[i] = (uintptr_t)NULL | ReadWriteFlag;
    }
    return res;
}

void deletePageStructure(uintptr_t* pageStruct) {
    if (getCR3() == pageStruct) {
        printf("Freeing page structure in CR3 -- something bad is about to happen!\n");
    }
    freePageFrame(pageStruct);
}
