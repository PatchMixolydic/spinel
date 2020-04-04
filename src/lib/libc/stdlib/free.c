#include <stdio.h>
#ifdef __KERNEL
#include <spinel/alloc.h>
#endif

void free(void* ptr) {
    #ifdef __KERNEL
        kfree(ptr);
    #else
        printf("free not implemented for hosted\n");
        (void)ptr;
    #endif // def __KERNEL
}
