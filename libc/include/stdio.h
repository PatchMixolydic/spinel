#ifndef STDIO_H
#define STDIO_H

#include <sys/cdefs.h>

#define EOF (-1)

#ifdef __cplusplus
extern "C" {
#endif

int printf(const char* __restrict format, ...);
int putchar(int ic);
int puts(const char* string);

#ifdef __cplusplus
}
#endif

#endif
