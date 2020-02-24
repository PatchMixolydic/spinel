#ifndef SPINEL_PANIC_H
#define SPINEL_PANIC_H

// TODO: vararg panic when printf added
void panic(const char why[]);

#ifndef DEBUG
#define kassert(expr, msg) do {\
    if (!(expr)) {\
        panic("Assertion failed in " __FILE__ ": " #expr);\
    }\
} while (0)
#else
#define kassert(expr, msg)
#endif

#endif // ndef SPINEL_PANIC_H
