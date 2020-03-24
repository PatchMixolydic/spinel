#include <stddef.h>
#include <string.h>

char* strcpy(char* dest, const char* src) {
    return strncpy(dest, src, strlen(src));
}
