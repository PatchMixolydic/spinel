#include <ctype.h>
#include <stdbool.h>
#include <stdint.h>
#include <multiboot2.h>
#include <spinel/panic.h>
#include <spinel/tty.h>
#include "../core/cpu.h"
#include "../core/gdt.h"
#include "../interrupts/idt.h"
#include "../peripherals/pic.h"
#include "multibootProc.h"

void archBoot(uint32_t multibootMagic, struct multiboot_tag* multibootInfo) {
    clearScreen();
    disableCursor();
    putString("Spinel booting for i386\n");
    initGDT();
    initIDT();
    picSetAllMasked(true);
    initPIC();
    enableInterrupts();
    picSetIRQMasked(1, false);
    processMultibootInfo(multibootInfo);
}
