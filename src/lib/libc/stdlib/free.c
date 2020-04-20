#include <stdio.h>
#ifdef __Kernel
#include <spinel/alloc.h>
#endif

void free(void* ptr) {
    #ifdef __Kernel
        kfree(ptr);
    #else
        printf("free not implemented for hosted\n");
        (void)ptr;
    #endif // def __Kernel
}
