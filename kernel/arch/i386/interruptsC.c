#include <stdio.h>
#include <kernel/panic.h>
#include "io.h"
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

void interruptHandler(uint8_t interrupt) {
    if (interrupt < 32) { // intel exception
        panic(ExceptionDescriptions[interrupt]);
    }
    panic("Exception!");
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
