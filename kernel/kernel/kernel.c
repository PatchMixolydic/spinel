#include <stdio.h>

#include <kernel/tty.h>

void kernelMain(void) {
	terminalInitialize();
	printf("The system is coming up.\n");
}
