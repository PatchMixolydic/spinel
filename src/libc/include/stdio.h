#ifndef _LIBC_STDIO_H
#define _LIBC_STDIO_H

#include <stdarg.h>

int printf(const char* format, ...);
int vprintf(const char* format, va_list parameters);

#endif // ndef _LIBC_STDIO_H