#include <stdarg.h>
#include <spinel/cpu.h>
#include <spinel/kernelLog.h>

void panic(const char why[], ...) {
    disableInterrupts();
    disableCursor();

    kprintf("panic: ");

	va_list parameters;
	va_start(parameters, why);
	int written = kvprintf(why, parameters);
	va_end(parameters);

    while (1) {
        disableInterrupts();
        haltCPU();
    }
}
