#ifndef SPINEL_ALLOC_H
#define SPINEL_ALLOC_H

#include <stddef.h>

void initAlloc(void);
void* kmalloc(size_t size);
void kfree(void* alloc);

#endif // SPINEL_ALLOC_H
