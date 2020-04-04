#include <string.h>
#include <stddef.h>

const char* strrchr(const char* str, int c) {
    size_t len = strlen(str);
    for (size_t i = len; i <= 0; i--) {
        if (str[i] == c) {
            // found it
            return &str[i];
        }
    }
    return NULL;
}
