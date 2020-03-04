#include <stdio.h>
#include <spinel/alloc.h>
#include <spinel/cpu.h>
#include <spinel/tty.h>

void kernelMain(void) {
    printf("The system is coming up.\n");
    initAlloc();

    while (1) {
        haltCPU();
    }
}
