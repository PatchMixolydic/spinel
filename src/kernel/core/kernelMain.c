#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <spinel/alloc.h>
#include <spinel/concurrency.h>
#include <spinel/cpu.h>
#include <spinel/tty.h>
#include <spinel/vfs.h>
#include <spinel/zero.h>

// Temporary, until modules exist
void nullfsInit(void);

void kernelMain(void) {
    printf("The system is coming up.\n");
    initAlloc();
    initVFS();
    nullfsInit();

    while (1) {
        haltCPU();
    }
}
