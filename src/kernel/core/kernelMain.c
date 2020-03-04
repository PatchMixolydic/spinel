#include <stdio.h>
#include <spinel/alloc.h>
#include <spinel/cpu.h>
#include <spinel/tty.h>

void kernelMain(void) {
    printf("The system is coming up.\n");
    initAlloc();
    printf("\x1b[30mH\x1b[31me\x1b[32ml\x1b[33ml\x1b[34mo \x1b[35mw\x1b[36mo\x1b[37mr\x1b[42mld\x1b[0m\n");

    while (1) {
        haltCPU();
    }
}
