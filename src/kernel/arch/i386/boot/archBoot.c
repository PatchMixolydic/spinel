#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <multiboot2.h>
#include <spinel/kernelInfo.h>
#include <spinel/pci.h>
#include <spinel/tty.h>
#include <spinel/arch/i386/cpu.h>
#include <spinel/arch/i386/fpu.h>
#include <spinel/arch/i386/gdt.h>
#include <spinel/arch/i386/idt.h>
#include <spinel/arch/i386/virtualMem.h>
#include <spinel/arch/i386/cmos.h>
#include <spinel/arch/i386/pic.h>
#include <spinel/arch/i386/pit.h>
#include <spinel/arch/i386/ps2.h>
#include <spinel/arch/i386/ps2Keyboard.h>
#include <spinel/arch/i386/serial.h>
#include <spinel/arch/i386/multibootProc.h>

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

    picSetAllMasked(true);
    initPIC();
    enableInterrupts();
    setNMIEnabled(true);

    initCMOS();

    initFPU();
    initPIT();

    initPS2USBLegacy();
    // picSetIRQMasked(1, false);

    processMultibootInfo(multibootInfo);
    initVirtualMemory();

    pciEnumerateDevices();
}
