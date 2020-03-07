#include <stddef.h>

#ifdef __LIBCKERN
#include <spinel/alloc.h>
#endif

void* malloc(size_t size) {
    #ifdef __LIBCKERN
        return kmalloc(size);
    #else
        #warning "malloc not implemented for hosted environment"
        (void)size;
        return NULL;
    #endif // def __LIBCKERN
}
