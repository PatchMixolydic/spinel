#include <stdio.h>
#include <kernel/interrupts.h>
#include <kernel/panic.h>

void panic(const char why[]) {
	disableInterrupts();
	printf("panic: %s", why);
	while (1);
}
