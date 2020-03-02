#include <stddef.h>
#include <string.h>

size_t strlcpy(char* dest, const char* src, size_t size) {
    int i;
    for (i = 0; i < size - 1 && src[i] != '\0'; i++) {
        dest[i] = src[i];
    }
    dest[i] = '\0';
    return strlen(src);
}
