#include <stddef.h>
#include <string.h>

char* strpbrk(const char* str, const char* delimiters) {
    size_t strLen = strlen(str);
    size_t delimitersLen = strlen(delimiters);

    // TODO: this is probably inefficient
    for (size_t i = 0; i < strLen; i++) {
        for (size_t j = 0; j < delimitersLen; j++) {
            if (str[i] == delimiters[j]) {
                // we've found a match
                return (char*)str + i;
            }
        }
    }

    return NULL;
}
