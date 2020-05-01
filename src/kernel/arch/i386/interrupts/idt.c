#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include "../core/cpu.h"
#include "../core/gdt.h"
#include "interrupts.h"

#define RegisterISR(num) do {\
    extern void isr##num(void);\
    uintptr_t offset = (uintptr_t)isr##num;\
    idt[num] = (InterruptDesc){\
        (uint16_t)(offset & 0xFFFF),\
        getGDTOffset(GDTKernelCode),\
        0,\
        /* present, ring 0, interrupt gate*/\
        0x8E, /* 0b1000_1110 */\
        (uint16_t)(offset >> 16)\
    };\
} while(0)

#define RegisterTenISRs(tensPlace) do {\
    RegisterISR(tensPlace##0); RegisterISR(tensPlace##1);\
    RegisterISR(tensPlace##2); RegisterISR(tensPlace##3);\
    RegisterISR(tensPlace##4); RegisterISR(tensPlace##5);\
    RegisterISR(tensPlace##6); RegisterISR(tensPlace##7);\
    RegisterISR(tensPlace##8); RegisterISR(tensPlace##9);\
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
static InterruptDesc idt[256];
static IDTPointer idtp = {sizeof(idt) - 1, idt};

void initIDT(void) {
    // Let's go ahead and clear the IDT lest anything strange happen
    memset((void*)idt, 0, sizeof(idt));
    // nb. C thinks anything starting with 0 is an octal constant
    RegisterTenISRs(/*0*/);
    RegisterTenISRs(1);
    RegisterTenISRs(2);
    RegisterTenISRs(3);
    RegisterISR(40);
    RegisterISR(41);
    RegisterISR(42);
    RegisterISR(43);
    RegisterISR(44);
    RegisterISR(45);
    RegisterISR(46);
    RegisterISR(0x88);

    loadIDT((uintptr_t)&idtp);
}

#undef RegisterTenISRs
#undef RegisterISR
