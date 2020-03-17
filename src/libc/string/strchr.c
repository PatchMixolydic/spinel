#include <string.h>
#include <stddef.h>

const char* strchr(const char* str, int c) {
    size_t len = strlen(str);
    for (size_t i = 0; i <= len; i++) {
        if (str[i] == c) {
            // found it
            return &str[i];
        }
    }
    return NULL;
}
