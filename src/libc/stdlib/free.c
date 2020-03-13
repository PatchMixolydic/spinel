#include <stdio.h>
#ifdef __LIBCKERN
#include <spinel/alloc.h>
#endif

void free(void* ptr) {
    #ifdef __LIBCKERN
        kfree(ptr);
    #else
        printf("free not implemented for hosted\n");
        (void)ptr;
    #endif // def __LIBCKERN
}
