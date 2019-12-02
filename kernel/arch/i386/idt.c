#include <stdint.h>
#include "interrupts.h"

typedef struct {
    uint16_t offsetLow;
    uint16_t gdtRegion; // where in the GDT is this? each GDT entry is 8 bytes
    uint8_t unused; // ist on amd64
    uint8_t typeAndAttributes;
    uint16_t offsetHigh; // offsetMid on amd64
    // uint32_t offsetHigh; // on amd64
    // uint32_t unused; // on amd64
} __attribute__((packed)) IDTEntry;

IDTEntry idt[256];
void (*irqPointer[16]) = { // array of function pointers
    irq0, irq1, irq2, irq3, irq4, irq5, irq6, irq7, irq8, irq9, irq10, irq11,
    irq12, irq13, irq14, irq15
};

void loadIDTEntry(int interrupt, uint16_t offset, uint16_t region, uint8_t typeAndAttributes) {
    idt[interrupt].offsetLow = offset & 0xFFFF;
    idt[interrupt].gdtRegion = region;
    idt[interrupt].unused = 0;
    idt[interrupt].typeAndAttributes = typeAndAttributes;
    idt[interrupt].offsetHigh = (offset >> 16) & 0xFFFF;
}

void initIDT() {
    for (int exception = 0; exception < 256; exception++) { // for each exception
        uint32_t handler = (uint32_t)exceptionHandler;
        loadIDTEntry(exception, handler, 0x08, 0x8E);
    }

    uint32_t handler = (uint32_t)doubleFault;
    loadIDTEntry(8, handler, 0x08, 0x8E);

    handler = (uint32_t)doubleFault;
    loadIDTEntry(13, handler, 0x08, 0x8E);

    for (int irq = 0; irq < 16; irq++) { // for each IRQ
        // Due to the way IRQs are set up, each IRQ's handler is 32 + irq
        int interrupt = irq + 32;
        uint32_t handler = (uint32_t)irqPointer[irq]; // pointer to irq handler
        loadIDTEntry(interrupt, handler, 0x08, 0x8E);
    }

    IDTRegister reg;
    reg.length = sizeof(IDTEntry) * 256 - 1;
    reg.idtPointer = (uint32_t)&idt;
    loadIDT(reg);
}
