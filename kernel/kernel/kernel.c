#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <kernel/memory.h>
#include <kernel/multiboot.h>
#include <kernel/panic.h>
#include <kernel/tty.h>

void kernelMain() {
	terminalDisableCursor();
	printf("The system is coming up.\n");
	while(1) allocatePageFrame();

	// panic("nevermind");
	while(1) __asm__("hlt");
}
