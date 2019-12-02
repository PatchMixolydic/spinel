#include <stdio.h>
#include <kernel/interrupts.h>
#include <kernel/panic.h>

void panic(const char why[]) {
	printf("panic: %s", why);
	disableInterrupts();
	while (1);
}
