#ifndef _LIBC_STRING_H
#define _LIBC_STRING_H

#include <stddef.h>

int memcmp(const void* aptr, const void* bptr, size_t size);
void* memcpy(void* restrict dstptr, const void* restrict srcptr, size_t size);
void* memmove(void* dstptr, const void* srcptr, size_t size);
void* memset(void* bufptr, int value, size_t size);

int strncmp(const char* s1, const char* s2, size_t size);
char* strdup(const char* s);
size_t strlcpy(char* dest, const char* src, size_t size);
size_t strlen(const char* str);

#endif // ndef _LIBC_STRING_H
