#include <spinel/tty.h>

void panic(const why[]) {
    disableCursor();
    putString("panic: ");
    putString(why);

    while (1) {
        haltCPU();
    }
}
