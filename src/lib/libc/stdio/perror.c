#include <errno.h>
#include <stdio.h>
#include <string.h>

void perror(const char* str) {
    // TODO: fprintf(stderr, 
    printf("%s: %s\n", str, strerror(errno));
}
