#ifndef STDIO_H
#define STDIO_H

#include <sys/cdefs.h>
#include <stdarg.h>

#define EOF (-1)

#ifdef __cplusplus
extern "C" {
#endif

int printf(const char* __restrict format, ...);
int putchar(int ic);
int puts(const char* string);
int vprintf(const char* format, va_list ap);

#ifdef __cplusplus
}
#endif

#endif
