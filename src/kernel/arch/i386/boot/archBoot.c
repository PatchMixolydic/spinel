#include <multiboot2.h>
#include <stdint.h>
#include <spinel/panic.h>
#include <spinel/tty.h>
#include "../core/cpu.h"
#include "../core/gdt.h"
#include "multibootProc.h"

void archBoot(uint32_t multibootMagic, struct multiboot_tag* multibootInfo) {
    clearScreen();
    putString("Spinel booting for i386\n");
    initGDT();
    processMultibootInfo(multibootInfo);
}
