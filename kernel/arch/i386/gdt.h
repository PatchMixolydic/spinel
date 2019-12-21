#ifndef ARCH_I386_GDT_H
#define ARCH_I386_GDT_H

#include <stdint.h>

// nb. This is also the home of TSS

typedef enum {
    GDTNull = 0,
    GDTKernelCode,
    GDTKernelData,
    GDTUserCode,
    GDTUserData,
    GDTTSS
} GDTEntries;

typedef struct {
    uint16_t sizeMinusOne;
    uint32_t offset;
} __attribute__((packed)) GDTDescriptor;

typedef struct {
    uint16_t link, reserved0;
    uint32_t esp0;
    uint16_t ss0, reserved1;
    uint32_t esp1;
    uint16_t ss1, reserved2;
    uint32_t esp2;
    uint16_t ss2, reserved3;
    uint32_t cr3, eip, eflags, eax, ecx, edx, ebx, esp, ebp, esi, edi;
    uint16_t es, reserved4;
    uint16_t cs, reserved5;
    uint16_t ss, reserved6;
    uint16_t ds, reserved7;
    uint16_t fs, reserved8;
    uint16_t gs, reserved9;
    uint16_t ldtr, reserved10;
    uint16_t reserved11, iopb;
} __attribute__((packed)) TaskStateSegment;

static TaskStateSegment tss;

void initGDT();
extern void loadGDT(GDTDescriptor* gdtDesc);

static inline uint16_t gdtOffset(int entry) {
    return entry * 0x8;
}

#endif // ndef ARCH_I386_GDT_H
