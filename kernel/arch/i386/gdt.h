#ifndef ARCH_I386_GDT_H
#define ARCH_I386_GDT_H

#include <stdint.h>

// nb. This is also the home of TSS

typedef struct {
    uint16_t sizeMinusOne;
    uint32_t offset;
} __attribute__((packed)) GDTDescriptor;

void initGDT();
extern void loadGDT(GDTDescriptor* gdtDesc);

#endif // ndef ARCH_I386_GDT_H
