#include <stdio.h>

#include <kernel/tty.h>

void kernelMain() {
	terminalInitialize();
	printf("The system is coming up.\n");
}
