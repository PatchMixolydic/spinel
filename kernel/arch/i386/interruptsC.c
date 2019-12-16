#include <stdio.h>
#include <kernel/panic.h>
#include "cpu.h"
#include "io.h"
#include "memory/paging.h"
#include "pic.h"

const char ExceptionDescriptions[][32] = {
    "Division by zero",
    "Debug",
    "Non-maskable interrupt",
    "Breakpoint",
    "Overflow",
    "Bound range exceeded",
    "Invalid opcode",
    "Device not available",
    "Double fault! :<",
    "Coprocessor segment overrun",
    "Invalid task state segment",
    "Segment not present",
    "Stack/segment fault",
    "General protection fault!",
    "Page fault",
    "Reserved",
    "x87 FPU exception",
    "Alignment check",
    "Machine check",
    "SIMD floating point exception",
    "Virtualization exception",
    "Reserved",
    "Security exception",
    "Reserved",
};

typedef enum {
    DivisionByZero, Debug, NMI, Breakpoint, Overflow, BoundRangeExceeded,
    InvalidOpcode, DeviceNotAvailable, DoubleFault, CoprocessorSegmentOverrun,
    InvalidTSS, SegmentNotPresent, StackSegmentFault, GeneralProtectionFault,
    PageFault, Reserved0, FPUException, AlignmentCheck, MachineCheck,
    SIMDFloatException, VirtualizationException, Reserved1, SecurityException,
    Reserved2
} IntelExceptions;

void interruptHandler(uint32_t interrupt, Registers regs, unsigned int errorCode, int eip, int cs, int eflags) {
    switch (interrupt) {
        case PageFault:
            handlePageFault(regs, errorCode);
            break;
        default:
            printf("Error code 0x%X\n", errorCode);
            printf("EAX\t0x%X\t\tEBX\t0x%X\t\tECX\t0x%X\t\tEDX\t0x%X\n", regs.eax, regs.ebx, regs.ecx, regs.edx);
            printf("ESP\t0x%X\t\tEBP\t0x%X\t\tESI\t0x%X\t\tEDI\t0x%X\n", regs.esp, regs.ebp, regs.esi, regs.edi);
            printf("EIP\t0x%X\t\tEFLAGS\t0x%X\n", eip, eflags);
            if (interrupt < 32) { // intel exception
                panic(ExceptionDescriptions[interrupt]);
            }
            panic("Exception!");
            break;
    }
}

void cIRQ0() {
    picEndOfInterrupt(0);
}

void cIRQ1() {
    uint8_t asdf = inb(0x60);
    printf("Keyboard 0x%X\n", asdf);
    picEndOfInterrupt(1);
}

void cIRQ2() {
    picEndOfInterrupt(2);
}

void cIRQ3() {
    picEndOfInterrupt(3);
}

void cIRQ4() {
    picEndOfInterrupt(4);
}

void cIRQ5() {
    picEndOfInterrupt(5);
}

void cIRQ6() {
    picEndOfInterrupt(6);
}

void cIRQ7() {
    picEndOfInterrupt(7);
}

void cIRQ8() {
    picEndOfInterrupt(8);
}

void cIRQ9() {
    picEndOfInterrupt(9);
}

void cIRQ10() {
    picEndOfInterrupt(10);
}

void cIRQ11() {
    picEndOfInterrupt(11);
}

void cIRQ12() {
    picEndOfInterrupt(12);
}

void cIRQ13() {
    picEndOfInterrupt(13);
}

void cIRQ14() {
    picEndOfInterrupt(14);
}

void cIRQ15() {
    picEndOfInterrupt(15);
}
