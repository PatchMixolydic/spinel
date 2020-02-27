#ifndef SPINEL_PANIC_H
#define SPINEL_PANIC_H

void panic(const char why[], ...);

#ifdef DEBUG
#define kassert(expr) do {\
    if (!(expr)) {\
        panic("Assertion failed at " __FILE__ ":%d: " #expr, __LINE__);\
    }\
} while (0)
#else
#define kassert(expr)
#endif

#endif // ndef SPINEL_PANIC_H
