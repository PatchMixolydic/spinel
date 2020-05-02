#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#ifdef __Kernel
#include <spinel/panic.h>
#else
#include <stdio.h>
#endif

void* calloc(size_t nmemb, size_t size) {
    size_t mallocSize;
    #ifdef __GNUC__
        // Use a GCC builtin to check for overflow
        if (!__builtin_mul_overflow(nmemb, size, &mallocSize)) {
            #ifdef __Kernel
                panic(
                    "calloc multiplication overflowed: "
                    "%u elements of size %u\n",
                    nmemb, size
                );
            #else
                // TODO: stderr
                printf(
                    "calloc multiplication overflowed: "
                    "%u elements of size %u\n",
                    nmemb, size
                );
                // TODO: abort
            #endif
        }
    #else
        // TODO: is there a way to check for overflow here?
        mallocSize = nmemb * size;
    #endif

    void* res = malloc(mallocSize);
    if (res != NULL) {
        memset(res, 0, mallocSize);
    }
    return res;
}
