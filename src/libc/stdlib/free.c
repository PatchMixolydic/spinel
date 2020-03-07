#ifdef __LIBCKERN
#include <spinel/alloc.h>
#endif

void free(void* ptr) {
    #ifdef __LIBCKERN
        kfree(ptr);
    #else
        #warning "free not implemented for hosted environment"
        (void)ptr;
    #endif // def __LIBCKERN
}
