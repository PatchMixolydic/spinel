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
    // TODO: clean this up!
    loadIDTEntry(0, (uintptr_t)isr0, 0x08, 0x8E);
    loadIDTEntry(1, (uintptr_t)isr1, 0x08, 0x8E);
    loadIDTEntry(2, (uintptr_t)isr2, 0x08, 0x8E);
    loadIDTEntry(3, (uintptr_t)isr3, 0x08, 0x8E);
    loadIDTEntry(4, (uintptr_t)isr4, 0x08, 0x8E);
    loadIDTEntry(5, (uintptr_t)isr5, 0x08, 0x8E);
    loadIDTEntry(6, (uintptr_t)isr6, 0x08, 0x8E);
    loadIDTEntry(7, (uintptr_t)isr7, 0x08, 0x8E);
    loadIDTEntry(8, (uintptr_t)isr8, 0x08, 0x8E);
    loadIDTEntry(9, (uintptr_t)isr9, 0x08, 0x8E);
    loadIDTEntry(10, (uintptr_t)isr10, 0x08, 0x8E);
    loadIDTEntry(11, (uintptr_t)isr11, 0x08, 0x8E);
    loadIDTEntry(12, (uintptr_t)isr12, 0x08, 0x8E);
    loadIDTEntry(13, (uintptr_t)isr13, 0x08, 0x8E);
    loadIDTEntry(14, (uintptr_t)isr14, 0x08, 0x8E);
    loadIDTEntry(15, (uintptr_t)isr15, 0x08, 0x8E);
    loadIDTEntry(16, (uintptr_t)isr16, 0x08, 0x8E);
    loadIDTEntry(17, (uintptr_t)isr17, 0x08, 0x8E);
    loadIDTEntry(18, (uintptr_t)isr18, 0x08, 0x8E);
    loadIDTEntry(19, (uintptr_t)isr19, 0x08, 0x8E);
    loadIDTEntry(20, (uintptr_t)isr20, 0x08, 0x8E);
    loadIDTEntry(21, (uintptr_t)isr21, 0x08, 0x8E);
    loadIDTEntry(22, (uintptr_t)isr22, 0x08, 0x8E);
    loadIDTEntry(23, (uintptr_t)isr23, 0x08, 0x8E);
    loadIDTEntry(24, (uintptr_t)isr24, 0x08, 0x8E);
    loadIDTEntry(25, (uintptr_t)isr25, 0x08, 0x8E);
    loadIDTEntry(26, (uintptr_t)isr26, 0x08, 0x8E);
    loadIDTEntry(27, (uintptr_t)isr27, 0x08, 0x8E);
    loadIDTEntry(28, (uintptr_t)isr28, 0x08, 0x8E);
    loadIDTEntry(29, (uintptr_t)isr29, 0x08, 0x8E);
    loadIDTEntry(30, (uintptr_t)isr30, 0x08, 0x8E);
    loadIDTEntry(31, (uintptr_t)isr31, 0x08, 0x8E);
    for (int i = 32; i < 48; i++) {
        loadIDTEntry(i, (uintptr_t)irqPointer[i], 0x08, 0x8E);
    }
    loadIDTEntry(80, (uintptr_t)isr80, 0x08, 0x8E);

    IDTRegister reg;
    reg.length = sizeof(idtBuffer) - 1;
    reg.idtPointer = (uintptr_t)idtBuffer;
    loadIDT(&reg);
}
