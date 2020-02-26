#include <spinel/kernelLog.h>
#include <spinel/panic.h>
#include "../core/cpu.h"
#include "../peripherals/pic.h"
#include "interrupts.h"

#define IRQPlaceholder(n) \
void irq##n(void) {\
    picEndOfInterrupt(n);\
}\

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

typedef enum {
    DivisionByZero, Debug, NMI,
    Breakpoint, Overflow, BoundRangeExceeded,
    InvalidOpcode, DeviceNotAvailable, DoubleFault,
    CoprocessorSegmentOverrun, InvalidTSS, SegmentNotPresent,
    StackSegmentFault, GeneralProtectionFault, PageFault,
    Reserved0, FPUException, AlignmentCheck,
    MachineCheck, SIMDFloatException, VirtualizationException,
    Reserved1, SecurityException, Reserved2
} IntelExceptions;

void interruptHandler(InterruptInfo info) {
    // TODO: vararg panic
    if (info.interruptNum < 32) {
        // CPU exception
        panic(ExceptionDescriptions[info.interruptNum]);
    }
    panic("Interrupt!");
}

IRQPlaceholder(0)

void irq1(void) {
    uint8_t scancode = inByte(0x60);
    kprintf("Got scan code 0x%X\n", scancode);
    picEndOfInterrupt(1);
}

IRQPlaceholder(2)
IRQPlaceholder(3)
IRQPlaceholder(4)
IRQPlaceholder(5)
IRQPlaceholder(6)
IRQPlaceholder(7)
IRQPlaceholder(8)
IRQPlaceholder(9)
IRQPlaceholder(10)
IRQPlaceholder(11)
IRQPlaceholder(12)
IRQPlaceholder(13)
IRQPlaceholder(14)
IRQPlaceholder(15)
