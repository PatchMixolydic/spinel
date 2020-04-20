#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <multiboot2.h>
#include <spinel/kernelInfo.h>
#include <spinel/pci.h>
#include <spinel/tty.h>
#include "../core/cpu.h"
#include "../core/fpu.h"
#include "../core/gdt.h"
#include "../interrupts/idt.h"
#include "../memory/virtualMem.h"
#include "../peripherals/cmos.h"
#include "../peripherals/pic.h"
#include "../peripherals/ps2.h"
#include "../peripherals/ps2Keyboard.h"
#include "../peripherals/serial.h"
#include "multibootProc.h"

void archBoot(uint32_t multibootMagic, struct multiboot_tag* multibootInfo) {
    clearScreen();
    disableCursor();

    initSerial(SerialMaxBaudRate);
    printf(
        "%s %s %s on %s %s\n",
        SpinelKernelName, SpinelReleaseVersion, SpinelBuildVerison,
        ProcessorName, MachineName
    );

    initGDT();
    initIDT();

    initCMOS();

    picSetAllMasked(true);
    initPIC();
    enableInterrupts();

    initPS2USBLegacy();
    // picSetIRQMasked(1, false);

    processMultibootInfo(multibootInfo);
    setupPageMaps();

    initFPU();
    pciEnumerateDevices();
}
