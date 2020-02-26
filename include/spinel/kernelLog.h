#ifndef KERNEL_LOG_H
#define KERNEL_LOG_H

#include <stdarg.h>

// TODO: may not stay, might be replaced when C library comes round
int kprintf(const char* format, ...);
int kvprintf(const char* format, va_list parameters);

#endif // ndef KERNEL_LOG_H
