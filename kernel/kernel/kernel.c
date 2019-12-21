#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <kernel/cpu.h>
#include <kernel/memory.h>
#include <kernel/multiboot.h>
#include <kernel/panic.h>
#include <kernel/scheduler.h>
#include <kernel/tty.h>

void kernelMain() {
	printf("The system is coming up.\n");
	initScheduler();
	printf("Press CTRL to panic.\n");

	while (true) {
		haltCPU();
	}
}
