#ifndef ARCH_I386_CPU_H
#define ARCH_I386_CPU_H

#include <stdint.h>

typedef struct {
    unsigned int edi, esi, ebp, esp;
    unsigned int ebx, edx, ecx, eax;
} Registers;

#endif // ndef ARCH_I386_CPU_H
