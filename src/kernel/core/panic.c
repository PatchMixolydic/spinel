#include <spinel/cpu.h>
#include <spinel/tty.h>

void panic(char why[]) {
    disableInterrupts();
    disableCursor();
    putString("panic: ");
    putString(why);

    while (1) {
        disableInterrupts();
        haltCPU();
    }
}
