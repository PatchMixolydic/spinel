#ifndef INTERRUPTS_H
#define INTERRUPTS_H

#include <stdint.h>

// This header serves two source files:
// interrupts.c, the implementation of interrupt handlers
// isr.asm, the actual interrupt service routines that the CPU calls

typedef struct {
    uint32_t interruptNum;
    uint32_t edi, esi, ebp, esp;
    uint32_t ebx, edx, ecx, eax;
    uint32_t eip, cs, eflags;
} InterruptInfo;

void interruptHandler(InterruptInfo info);

// If anyone knows of a better way to do this, please let me know...

extern void isr0(void); extern void isr1(void); extern void isr2(void);
extern void isr3(void); extern void isr4(void); extern void isr5(void);
extern void isr6(void); extern void isr7(void); extern void isr8(void);
extern void isr9(void); extern void isr10(void); extern void isr11(void);
extern void isr12(void); extern void isr13(void); extern void isr14(void);
extern void isr15(void); extern void isr16(void); extern void isr17(void);
extern void isr18(void); extern void isr19(void); extern void isr20(void);
extern void isr21(void); extern void isr22(void); extern void isr23(void);
extern void isr24(void); extern void isr25(void); extern void isr26(void);
extern void isr27(void); extern void isr28(void); extern void isr29(void);
extern void isr30(void); extern void isr31(void);

extern void irqISR0(void); extern void irqISR1(void);
extern void irqISR2(void); extern void irqISR3(void);
extern void irqISR4(void); extern void irqISR5(void);
extern void irqISR6(void); extern void irqISR7(void);
extern void irqISR8(void); extern void irqISR9(void);
extern void irqISR10(void); extern void irqISR11(void);
extern void irqISR12(void); extern void irqISR13(void);
extern void irqISR14(void); extern void irqISR15(void);

extern void isr80(void);

#endif // ndef INTERRUPTS_H
