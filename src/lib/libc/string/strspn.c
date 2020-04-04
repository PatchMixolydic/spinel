#include <stddef.h>
#include <string.h>

size_t strspn(const char* str, const char* chars) {
    size_t res = 0;
    size_t strLen = strlen(str);
    size_t charsLen = strlen(chars);

    for (size_t i = 0; i < strLen; i++) {
        for (size_t j = 0; j < charsLen; j++) {
            if (str[i] != chars[j]) {
                // end of span
                return res;
            }
        }
        // no match
        res++;
    }

    return res;
}
