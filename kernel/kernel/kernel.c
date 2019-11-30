#include <stdio.h>
#include <kernel/tty.h>

void kernelMain() {
	printf("The system is coming up.\n");
	panic("nevermind");
}
