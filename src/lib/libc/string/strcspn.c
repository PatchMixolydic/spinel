#include <stddef.h>
#include <string.h>

size_t strcspn(const char* str, const char* delimiters) {
    size_t res = 0;
    size_t strLen = strlen(str);
    size_t delimitersLen = strlen(delimiters);

    for (size_t i = 0; i < strLen; i++) {
        for (size_t j = 0; j < delimitersLen; j++) {
            if (str[i] == delimiters[j]) {
                // a match!
                return res;
            }
        }
        // no match
        res++;
    }

    return res;
}
