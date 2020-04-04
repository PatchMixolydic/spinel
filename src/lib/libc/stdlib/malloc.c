#include <stddef.h>
#include <stdio.h>

#ifdef __KERNEL
#include <spinel/alloc.h>
#endif

void* malloc(size_t size) {
    #ifdef __KERNEL
        return kmalloc(size);
    #else
        printf("realloc not implemented for hosted\n");
        (void)size;
        return NULL;
    #endif // def __KERNEL
}
