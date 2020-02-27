#include <stdarg.h>
#include <stdio.h>
#include <spinel/cpu.h>
#include <spinel/tty.h>

void panic(const char why[], ...) {
    disableInterrupts();
    disableCursor();

    printf("panic: ");

	va_list parameters;
	va_start(parameters, why);
	vprintf(why, parameters);
	va_end(parameters);

    while (1) {
        disableInterrupts();
        haltCPU();
    }
}
