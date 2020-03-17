#include <stddef.h>

int strncmp(const char* s1, const char* s2, size_t size) {
    size_t i;
    for (i = 0; i < size && s1[i] != '\0' && s2[i] != '\0'; i++) {
        if (s1[i] - s2[i] != 0) {
            return s1[i] - s2[i];
        }
    }

    if (i < size && s1[i] != s2[i] && (s1[i] != '\0' || s2[i] != '\0')) {
        // one terminates before the other. whichever one terminates is lesser
        // since their value at that point is 0
        return s1[i] == '\0' ? -1 : 1;
    }

    return 0;
}
