#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <kernel/cpu.h>
#include <kernel/memory.h>
#include <kernel/multiboot.h>
#include <kernel/panic.h>
#include <kernel/tty.h>

void kernelMain() {
	terminalDisableCursor();
	printf("The system is coming up.\n");
	printf("Press left CTRL to panic.\n");

	while (true) {
		haltCPU();
	}
}
