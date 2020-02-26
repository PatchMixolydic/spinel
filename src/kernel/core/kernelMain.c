#include <spinel/cpu.h>
#include <spinel/tty.h>

void kernelMain(void) {
    putString("The system is coming up.\n");
    volatile int s = *(int*)NULL;

    while (1) {
        haltCPU();
    }
}
