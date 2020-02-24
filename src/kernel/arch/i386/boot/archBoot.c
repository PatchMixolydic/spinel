#include <multiboot2.h>
#include <stdint.h>
#include <spinel/panic.h>
#include <spinel/tty.h>
#include "../core/cpu.h"

void archBoot(uint32_t multibootMagic, struct multiboot_tag* multibootInfo) {
    putString("Spinel booting for i386\n");
}
