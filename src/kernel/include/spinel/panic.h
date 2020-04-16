#ifndef SPINEL_PANIC_H
#define SPINEL_PANIC_H

#include <stdbool.h>

void panic(const char why[], ...);
bool isPanicking(void);

#endif // ndef SPINEL_PANIC_H
