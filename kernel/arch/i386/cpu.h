#ifndef ARCH_I386_CPU_H
#define ARCH_I386_CPU_H

#include <stdint.h>

typedef struct {
    uint32_t edi, esi, ebp, esp;
    uint32_t ebx, edx, ecx, eax;
} Registers;

#endif // ndef ARCH_I386_CPU_H
