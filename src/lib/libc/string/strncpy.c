#include <stddef.h>

char* strncpy(char* dest, char* src, size_t size) {
    size_t i;
    for (i = 0; i < size && src[i] != '\0'; i++) {
        dest[i] = src[i];
    }
    if (i < (size - 1)) {
        // must write size bytes to dest
        for (; i < size; i++) {
            dest[i] = '\0';
        }
    }
    return dest;
}
