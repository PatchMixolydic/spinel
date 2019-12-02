#include <stdio.h>
#include <kernel/panic.h>
#include "io.h"
#include "pic.h"

void cException() {
    panic("Exception!");
}

void cDoubleFault() {
    panic("Double fault!");
}

void cGeneralProtectionFault() {
    panic("General protection fault!");
}

void cInt80() {
    printf("Int 80!\n");
}

void cIRQ0() {
    picEndOfInterrupt(0);
}

void cIRQ1() {
    uint16_t asdf = inb(0x60);
    panic("Keyboard");
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
