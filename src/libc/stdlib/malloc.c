#include <spinel/alloc.h>

void* malloc(size_t size) {
    #ifdef __LIBCKERN
        return kmalloc(size);
    #else
        // TODO
        return NULL;
    #endif // def __LIBCKERN
}

void free(void* ptr) {
    #ifdef __LIBCKERN
        kfree(ptr);
    #else
        // TODO
    #endif // def __LIBCKERN
}
