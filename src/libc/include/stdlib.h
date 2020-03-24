#ifndef _LIBC_STDLIB_H
#define _LIBC_STDLIB_H

#include <stddef.h>

char* itoa(int value, char* str, int base);
char* uitoa(unsigned value, char* str, int base);

void* calloc(size_t nmemb, size_t size);
void* malloc(size_t size);
void* realloc(void* ptr, size_t size);
void free(void* ptr);

#endif // ndef _LIBC_STDLIB_H
