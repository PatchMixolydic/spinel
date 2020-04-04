#include <stddef.h>
#include <stdlib.h>

// TODO: This is wrong! This should avoid overflow
void* calloc(size_t nmemb, size_t size) {
    return malloc(nmemb * size);
}
