#include <stdint.h>
#include <multiboot2.h>
#include "../core/cpu.h"

void archBoot(uint32_t multibootMagic, struct multiboot_tag* multibootInfo) {
    // Temporary poc test
    uint16_t* vgaMemory = (uint16_t*)0xC00B8000;
    if (multibootMagic == MULTIBOOT2_BOOTLOADER_MAGIC) {
        vgaMemory[0] = 0x0F4F;
    } else {
        vgaMemory[0] = 0x0F58;
    }
    while (1) {
        haltCPU();
    }
}
