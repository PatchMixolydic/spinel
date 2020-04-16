#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <spinel/cpu.h>
#include <spinel/tty.h>

static bool panicking = false;

bool isPanicking(void) {
    return panicking;
}

void panic(const char why[], ...) {
    if (isPanicking()) {
        printf("panicception!: ");
        va_list parameters;
        va_start(parameters, why);
        vprintf(why, parameters);
        va_end(parameters);

        printf("\n");
    } else {
        disableInterrupts();
        panicking = true;
        disableCursor();

        printf("panic: ");

        va_list parameters;
        va_start(parameters, why);
        vprintf(why, parameters);
        va_end(parameters);

        printf("\n");
        printBacktrace();
    }

    while (1) {
        disableInterrupts();
        haltCPU();
    }
}
