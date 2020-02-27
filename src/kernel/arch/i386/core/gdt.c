#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include "cpu.h"
#include "gdt.h"

static const uint8_t GDTPresent = 1 << 7;
static const uint8_t GDTUserMode = 3 << 5;
static const uint8_t GDTNotTSS = 1 << 4;
static const uint8_t GDTExecutable = 1 << 3;
static const uint8_t GDTGrowDown = 1 << 2;
static const uint8_t GDTConforming = 1 << 2;
static const uint8_t GDTReadWrite = 1 << 1;
static const uint8_t GDTAccessed = 1;
static const uint8_t GDTPageGranularity = 1 << 7;
static const uint8_t GDT32Bit = 1 << 6;
static const uint8_t GDT64Bit = 1 << 5;

typedef struct {
    uint16_t limitLow;
    uint16_t baseLow;
    uint8_t baseMid;
    uint8_t access;
    uint8_t flagsLimitHigh;
    uint8_t baseHigh;
} __attribute__((packed)) GDTDescriptor;

typedef struct {
    uint16_t sizeMinusOne;
    uintptr_t offset;
} __attribute__((packed)) GDTPointer;

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
static GDTDescriptor gdt[6];
static GDTPointer gdtPointer;

uint16_t getGDTOffset(GDTEntry entry) {
    return (uint16_t)(entry * 0x8);
}

void initGDT(void) {
    // Clear our data structures
    memset((void*)gdt, 0, sizeof(GDTDescriptor) * 6);
    memset((void*)&tss, 0, sizeof(TaskStateSegment));

    gdt[GDTKernelCode] = (GDTDescriptor){
        0xFFFF, 0, 0, GDTPresent | GDTNotTSS | GDTExecutable | GDTReadWrite,
        GDTPageGranularity | GDT32Bit | 0xF, 0
    };
    gdt[GDTKernelData] = (GDTDescriptor){
        0xFFFF, 0, 0, GDTPresent | GDTNotTSS | GDTReadWrite,
        GDTPageGranularity | GDT32Bit | 0xF, 0
    };
    gdt[GDTUserCode] = (GDTDescriptor){
        0xFFFF, 0, 0,
        GDTPresent | GDTUserMode | GDTNotTSS | GDTExecutable | GDTReadWrite,
        GDTPageGranularity | GDT32Bit | 0xF, 0
    };
    gdt[GDTUserData] = (GDTDescriptor){
        0xFFFF, 0, 0,
        GDTPresent | GDTUserMode | GDTNotTSS | GDTReadWrite,
        GDTPageGranularity | GDT32Bit | 0xF, 0
    };

    // TSS descriptor
    GDTDescriptor* tssEntry = &gdt[GDTTSS];
    size_t tssSize = sizeof(tss);
    uintptr_t tssAddr = (uintptr_t)&tss;
    tssEntry->baseLow = tssAddr & 0xFFFF;
    tssEntry->baseMid = (tssAddr & 0xFF0000) >> 16;
    tssEntry->baseHigh = (tssAddr & 0xFF000000) >> 24;
    tssEntry->limitLow = (tssSize & 0xFFFF);
    tssEntry->flagsLimitHigh = (tssSize & 0xF0000) >> 16;
    tssEntry->flagsLimitHigh |= GDT32Bit;
    // TODO: verify the following
    tssEntry->access = GDTPresent | GDTExecutable | GDTAccessed;

    // Initialize the TSS
    tss.ss0 = getGDTOffset(GDTKernelData);
    tss.esp0 = getESP(); // Kernel stack to use -- this will get replaced upon context switch
    tss.iopb = tssSize; // Apparently this value works just fine

    // Let's go
    gdtPointer.sizeMinusOne = sizeof(gdt) - 1;
    gdtPointer.offset = (uintptr_t)gdt;
    loadGDT((uintptr_t)&gdtPointer);
}
