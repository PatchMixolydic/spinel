#ifndef ARCH_I386_INTERRUPTS_H
#define ARCH_I386_INTERRUPTS_H

#include <stdint.h>

typedef struct {
    uint16_t length;
    uint32_t idtPointer;
} __attribute__((packed)) IDTRegister;

extern void loadIDT(IDTRegister reg);
extern void enableInterrupts();
extern void disableInterrupts();
extern void exceptionHandler();
extern void doubleFault();
extern void generalProtectionFault();
extern void int80();
extern void irq0();
extern void irq1();
extern void irq2();
extern void irq3();
extern void irq4();
extern void irq5();
extern void irq6();
extern void irq7();
extern void irq8();
extern void irq9();
extern void irq10();
extern void irq11();
extern void irq12();
extern void irq13();
extern void irq14();
extern void irq15();

#endif // ndef ARCH_I386_INTERRUPTS_H
