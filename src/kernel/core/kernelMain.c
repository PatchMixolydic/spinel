#include <stdio.h>
#include <spinel/alloc.h>
#include <spinel/cpu.h>
#include <spinel/tty.h>

void kernelMain(void) {
    printf("The system is coming up.\n");
    initAlloc();
    printf("\x1b[34mHello\x1b[0m world\n");

    while (1) {
        haltCPU();
    }
}
