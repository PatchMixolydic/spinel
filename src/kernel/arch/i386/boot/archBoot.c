#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <multiboot2.h>
#include <spinel/kernelInfo.h>
#include <spinel/tty.h>
#include "../core/cpu.h"
#include "../core/gdt.h"
#include "../interrupts/idt.h"
#include "../memory/virtualMem.h"
#include "../peripherals/pic.h"
#include "multibootProc.h"

void archBoot(uint32_t multibootMagic, struct multiboot_tag* multibootInfo) {
    clearScreen();
    disableCursor();
    printf(
        "%s %s %s on %s %s\n",
        SpinelKernelName, SpinelReleaseVersion, SpinelBuildVerison,
        ProcessorName, MachineName
    );

    initGDT();
    initIDT();

    picSetAllMasked(true);
    initPIC();
    enableInterrupts();
    picSetIRQMasked(1, false);

    processMultibootInfo(multibootInfo);
    setupPageMaps();
}
