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
    uint32_t errorCode, eip, cs, eflags;
} InterruptInfo;

void interruptHandler(InterruptInfo info);

#endif // ndef INTERRUPTS_H
