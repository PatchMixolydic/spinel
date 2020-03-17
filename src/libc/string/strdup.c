#include <stddef.h>
#include <stdlib.h>
#include <string.h>

char* strdup(const char* str) {
    // TODO: calloc
    // be sure to account for nul terminator
    size_t len = strlen(str) + 1;
    char* res = malloc(sizeof(char) * len);

    if (res == NULL) {
        // TODO: ENOMEM
        return NULL;
    }

    strlcpy(res, str, len);
    return res;
}
