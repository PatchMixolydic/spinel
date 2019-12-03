#include <stdint.h>
#include "interrupts.h"

typedef struct {
    uint16_t offsetLow;
    uint16_t selector; // where in the GDT is this? each GDT entry is 8 bytes
    uint8_t unused; // ist on amd64
    uint8_t typeAndAttributes;
    uint16_t offsetHigh; // offsetMid on amd64
    // uint32_t offsetHigh; // on amd64
    // uint32_t unused; // on amd64
} __attribute__((packed)) IDTEntry;

IDTEntry idtBuffer[256];
void (*irqPointer[16]) = { // array of function pointers
    irq0, irq1, irq2, irq3, irq4, irq5, irq6, irq7, irq8, irq9, irq10, irq11,
    irq12, irq13, irq14, irq15
};

void loadIDTEntry(int interrupt, uintptr_t offset, uint16_t selector, uint8_t typeAndAttributes) {
    idtBuffer[interrupt].offsetLow = offset & 0xFFFF;
    idtBuffer[interrupt].selector = selector;
    idtBuffer[interrupt].unused = 0;
    idtBuffer[interrupt].typeAndAttributes = typeAndAttributes;
    idtBuffer[interrupt].offsetHigh = (offset >> 16) & 0xFFFF;
}

void initIDT() {
    for (int i = 0; i < 256; i++) {
        loadIDTEntry(i, 0, 0, 0);
    }
    for (int i = 0; i < 32; i++) {
        loadIDTEntry(i, (uintptr_t)exceptionHandler, 0x08, 0x8E);
    }
    for (int i = 32; i < 48; i++) {
        loadIDTEntry(i, (uintptr_t)irqPointer[i], 0x08, 0x8E);
    }
    loadIDTEntry(8, (uintptr_t)doubleFault, 0x08, 0x8E);
    loadIDTEntry(0xD, (uintptr_t)generalProtectionFault, 0x08, 0x8E);
    loadIDTEntry(80, (uintptr_t)int80, 0x08, 0x8E);

    IDTRegister reg;
    reg.length = sizeof(idtBuffer) - 1;
    reg.idtPointer = (uintptr_t)idtBuffer;
    loadIDT(&reg);
}
