#include <stdio.h>
#include <spinel/alloc.h>
#include <spinel/clock.h>
#include <spinel/cpu.h>
#include <spinel/pci.h>
#include <spinel/random.h>
#include <spinel/vfs.h>

void kernelMain(void) {
    printf("The system is coming up.\n");
    initAlloc();
    // initVFS();
    initRandom();
    clockCreateAlarms();

    while (1) {
        haltCPU();
    }
}
