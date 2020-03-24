#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <spinel/alloc.h>
#include <spinel/concurrency.h>
#include <spinel/cpu.h>
#include <spinel/tty.h>
#include <spinel/zero.h>
#include <spinel/vfs.h>

void kernelMain(void) {
    printf("The system is coming up.\n");
    initAlloc();
    initVFS();

    while (1) {
        haltCPU();
    }
}
