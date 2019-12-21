#ifndef ARCH_I386_CPU_H
#define ARCH_I386_CPU_H

#include <stdint.h>

typedef struct {
    unsigned int edi, esi, ebp, esp;
    unsigned int ebx, edx, ecx, eax;
} ExceptionRegisters;

typedef struct {
    unsigned int cr3, ds, ss;
    unsigned int eflags;
    unsigned int edi, esi, ebp, esp;
    unsigned int ebx, edx, ecx, eax;
    // Other segment registers are assumed to be ds
    unsigned int eip, cs;
} Registers;

typedef struct {
    Registers regs;
    uintptr_t userStack;
    uintptr_t kernelStack;
} x86State;

extern uintptr_t getESP();

#endif // ndef ARCH_I386_CPU_H
