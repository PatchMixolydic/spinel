#ifndef STRING_H
#define STRING_H 1

#include <stddef.h>
#include <sys/cdefs.h>

#ifdef __cplusplus
extern "C" {
#endif

int memcmp(const void* aptr, const void* bptr, size_t size);
void* memcpy(void* __restrict dstptr, const void* __restrict srcptr, size_t size);
void* memmove(void* dstptr, const void* srcptr, size_t size);
void* memset(void* bufptr, int value, size_t size);
size_t strlen(const char* str);

#ifdef __cplusplus
}
#endif

#endif
