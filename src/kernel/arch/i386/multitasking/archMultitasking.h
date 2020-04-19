#ifndef ARCHMULTITASKING_H
#define ARCHMULTITASKING_H

#include <stdint.h>

typedef struct ArchTask {
    void* esp;
    void* kernelESP;
    void* cr3;
} ArchTask;

extern void x86TaskSwitch(ArchTask* task);

extern ArchTask* currentArchTask;

#endif // ndef ARCHMULTITASKING_H
