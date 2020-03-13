#include <stddef.h>
#include <stdlib.h>
#include <string.h>

char* strdup(const char* s) {
    // TODO: calloc
    size_t len = strlen(s);
    char* res = malloc(sizeof(char) * len);

    if (res == NULL) {
        // TODO: ENOMEM
        return NULL;
    }

    strlcpy(res, s, len);
    return res;
}
