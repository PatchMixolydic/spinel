#ifndef SPINEL_PANIC_H
#define SPINEL_PANIC_H

// TODO: vararg panic when printf added
void panic(char why[]);

#ifdef DEBUG
#define kassert(expr) do {\
    if (!(expr)) {\
        panic("Assertion failed in " __FILE__ ": " #expr);\
    }\
} while (0)
#else
#define kassert(expr)
#endif

#endif // ndef SPINEL_PANIC_H
