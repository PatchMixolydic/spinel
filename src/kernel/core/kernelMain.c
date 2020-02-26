#include <spinel/cpu.h>
#include <spinel/tty.h>

void kernelMain(void) {
    putString("The system is coming up.\n");

    while (1) {
        haltCPU();
    }
}
