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
