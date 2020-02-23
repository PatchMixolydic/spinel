#include <stdint.h>
#include "cpu.h"

void archBoot() {
    // Temporary poc test
    uint16_t* vgaMemory = (uint16_t*)0xC00B8000;
    vgaMemory[0] = 0x0F41;
    while (1) {
        haltCPU();
    }
}
