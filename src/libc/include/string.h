#ifndef _LIBC_STRING_H
#define _LIBC_STRING_H

#include <stddef.h>

int memcmp(const void* aptr, const void* bptr, size_t size);
void* memcpy(void* restrict dstptr, const void* restrict srcptr, size_t size);
void* memmove(void* dstptr, const void* srcptr, size_t size);
void* memset(void* bufptr, int value, size_t size);

int strncmp(const char* s1, const char* s2, size_t size);
size_t strcspn(const char* str, const char* delimiters);
char* strdup(const char* str);
size_t strlcpy(char* dest, const char* src, size_t size);
size_t strlen(const char* str);
char* strpbrk(const char* str, const char* delimiters);
const char* strchr(const char* str, int c);
const char* strrchr(const char* str, int c);
size_t strspn(const char* str, const char* chars);
char* strtok_r(char* str, const char* delimiters, char** state);

#endif // ndef _LIBC_STRING_H
