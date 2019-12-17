#include <stdio.h>
#include <kernel/panic.h>
#include "cpu.h"
#include "io.h"
#include "memory/paging.h"
#include "pic.h"

// Used to align the register printout
#define AnsiToCol2 "\x1B[16G"
#define AnsiToCol3 "\x1B[32G"
#define AnsiToCol4 "\x1B[48G"

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
        case PageFault: {
            handlePageFault(regs, errorCode);
            break;
        }

        default: {
            char* exceptionDesc = "Unknown interrupt";
            if (interrupt < 32) { // intel exception
                exceptionDesc = ExceptionDescriptions[interrupt];
            }
            panic(
                "%s (0x%X)\n"
                "Error code 0x%X\n"
                "EAX 0x%X" AnsiToCol2
                "EBX 0x%X" AnsiToCol3
                "ECX 0x%X" AnsiToCol4
                "EDX 0x%X\n"
                "ESP 0x%X" AnsiToCol2
                "EBP 0x%X" AnsiToCol3
                "ESI 0x%X" AnsiToCol4
                "EDI 0x%X\n"
                "EIP 0x%X" AnsiToCol2
                "EFLAGS 0x%X\n",
                exceptionDesc, interrupt, errorCode,
                regs.eax, regs.ebx, regs.ecx, regs.edx,
                regs.esp, regs.ebp, regs.esi, regs.edi,
                eip, eflags
            );
            break;
        }
    }
}

void cIRQ0() {
    picEndOfInterrupt(0);
}

void cIRQ1() {
    uint8_t asdf = inb(0x60);
    if (asdf == 0x1D) {
        // left ctrl
        panic("Keyboard 0x%X\n", asdf);
    } else {
        printf("Keyboard 0x%X\n", asdf);
    }
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
