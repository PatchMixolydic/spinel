#include <assert.h>
#include <stddef.h>
#include <stdio.h>
#include <spinel/panic.h>
#include "../core/cpu.h"
#include "../memory/virtualMem.h"
#include "../peripherals/pic.h"
#include "interrupts.h"

static const char ExceptionDescriptions[][32] = {
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

static IRQHandler irqHandlers[256];
static InterruptInfo* currentInterruptInfo = NULL;

void registerInterruptHandler(Interrupt interrupt, IRQHandler handler) {
    if (irqHandlers[interrupt] != NULL) {
        panic(
            "Tried to register multiple IRQ handlers for interrupt 0x%X!",
            interrupt
        );
    }
    irqHandlers[interrupt] = handler;
}

void mainInterruptHandler(InterruptInfo info) {
    currentInterruptInfo = &info;
    // in case this is an IRQ
    uint32_t irqNum = info.interruptNum - IntIRQ0;

    if (irqHandlers[info.interruptNum] != NULL) {
        irqHandlers[info.interruptNum]();
        picEndOfInterrupt(irqNum);
    } else {
        if (IntIRQ0 <= info.interruptNum && info.interruptNum <= IntIRQ15) {
            // We have an IRQ, just leave
            picEndOfInterrupt(irqNum);
            return;
        }

        const char* message = "Unknown interrupt";
        if (info.interruptNum < 32) {
            // CPU exception
            message = ExceptionDescriptions[info.interruptNum];
        }
        panic(
            "%s\n"
            "eax: 0x%X    ebx: 0x%X    ecx: 0x%X    edx: 0x%X\n"
            "esp: 0x%X    ebp: 0x%X    esi: 0x%X    edi: 0x%X\n"
            "eip: 0x%X    cs: 0x%X     eflags: 0x%X\n"
            "interrupt: 0x%X    error code: 0x%X",
            message, info.eax, info.ebx, info.ecx, info.edx,
            info.esp, info.ebp, info.esi, info.edi,
            info.eip, info.cs, info.eflags, info.interruptNum,
            info.errorCode
        );
    }
}

InterruptInfo* getInterruptInfo(void) {
    assert(currentInterruptInfo);
    return currentInterruptInfo;
}
