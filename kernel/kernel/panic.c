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

#ifndef NDEBUG
void _kassert(bool expr, const char strExpr[], const char file[], int line) {
	if (!expr) {
		panic("Assertion failed at %s:%d\n       %s", file, line, strExpr);
	}
}
#endif
