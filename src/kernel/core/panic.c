#include <stdarg.h>
#include <spinel/cpu.h>
#include <spinel/kernelLog.h>
#include <spinel/tty.h>

void panic(const char why[], ...) {
    disableInterrupts();
    disableCursor();

    kprintf("panic: ");

	va_list parameters;
	va_start(parameters, why);
	kvprintf(why, parameters);
	va_end(parameters);

    while (1) {
        disableInterrupts();
        haltCPU();
    }
}
