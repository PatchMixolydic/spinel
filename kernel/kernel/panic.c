#include <stdio.h>
#include <kernel/panic.h>

void panic(const char why[]) {
	printf("panic: %s", why);
	while (1);
}
