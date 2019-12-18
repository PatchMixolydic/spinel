#ifndef _KERNEL_PANIC_H
#define _KERNEL_PANIC_H

#include <stdbool.h>

void panic(const char why[], ...);

#ifdef NDEBUG
#define kassert(expr)
#else
void _kassert(bool expr, const char strExpr[], const char file[], int line);
#define kassert(expr) _kassert(expr, #expr, __FILE__, __LINE__)
#endif

#endif // ndef _KERNEL_PANIC_H
