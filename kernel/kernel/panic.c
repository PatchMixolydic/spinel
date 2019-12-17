#include <stdio.h>
#include <kernel/cpu.h>
#include <kernel/interrupts.h>
#include <kernel/panic.h>

void panic(const char why[], ...) {
	disableInterrupts();
	printf("panic: ");
	va_list parameters;
	va_start(parameters, why);
	vprintf(why, parameters);
	va_end(parameters);
	while (1) {
		haltCPU();
	}
}
