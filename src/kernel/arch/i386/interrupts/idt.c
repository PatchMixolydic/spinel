#include <stddef.h>
#include <stdint.h>
#include "../core/cpu.h"
#include "../core/gdt.h"
#include "interrupts.h"

#define RegisterInterrupt(num) do {\
    uintptr_t offset = (uintptr_t)isr##num;\
    idt[num] = (InterruptDesc){\
        (uint16_t)(offset & 0xFFFF),\
        getGDTOffset(GDTKernelCode),\
        0,\
        /* present, ring 0, interrupt gate*/\
        0x8E, /* 0b0000_0000_1000_1110 */\
        (uint16_t)(offset >> 16)\
    };\
} while(0)

#define RegisterTenInterrupts(tensPlace) do {\
    RegisterInterrupt(tensPlace##0); RegisterInterrupt(tensPlace##1);\
    RegisterInterrupt(tensPlace##2); RegisterInterrupt(tensPlace##3);\
    RegisterInterrupt(tensPlace##4); RegisterInterrupt(tensPlace##5);\
    RegisterInterrupt(tensPlace##6); RegisterInterrupt(tensPlace##7);\
    RegisterInterrupt(tensPlace##8); RegisterInterrupt(tensPlace##9);\
} while(0)

typedef struct {
    uint16_t offsetLow; // location of interrupt service routine
    uint16_t selector; // selects segment in GDT
    uint8_t unused;
    // byte 7 = present, 6 & 5 = privilege, 4 = storage segment, 3 - 0 = type
    uint8_t typeAndAttr;
    uint16_t offsetHigh;
} __attribute__((__packed__)) InterruptDesc;

typedef struct {
    uint16_t lengthMinusOne; // in bytes, 1 interrupt desc. = 8 bytes
    InterruptDesc* base;
} __attribute__((__packed__)) IDTPointer;

// There can be a maximum of 256 interrupts.
const size_t IDTSize = (256 * sizeof(InterruptDesc));
static InterruptDesc idt[256];
static IDTPointer idtp = {IDTSize - 1, idt};

void initIDT(void) {
    // TODO: memset
    // Let's go ahead and clear the IDT lest anything strange happen
    for (size_t i = 0; i < IDTSize; i++) {
        ((uint8_t*)idt)[i] = 0;
    }
    // nb. C thinks anything starting with 0 is an octal constant
    RegisterTenInterrupts(/*0*/);
    RegisterTenInterrupts(1);
    RegisterTenInterrupts(2);
    RegisterInterrupt(30);
    RegisterInterrupt(31);
    RegisterInterrupt(80);
    loadIDT((uintptr_t)&idtp);
}

#undef RegisterTenInterrupts
#undef RegisterInterrupt
