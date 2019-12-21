#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include "memory/paging.h"
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
} __attribute__((packed)) GDTEntry;

static GDTEntry gdt[6];
static GDTDescriptor gdtDesc;

void initGDT() {
    // Clear our data structures
    memset(gdt, 0, sizeof(GDTEntry) * 6);
    memset(&tss, 0, sizeof(TaskStateSegment));

    gdt[GDTKernelCode] = (GDTEntry){
        0xFFFF, 0, 0, GDTPresent | GDTNotTSS | GDTExecutable | GDTReadWrite,
        GDTPageGranularity | GDT32Bit | 0xF, 0
    };
    gdt[GDTKernelData] = (GDTEntry){
        0xFFFF, 0, 0, GDTPresent | GDTNotTSS | GDTReadWrite,
        GDTPageGranularity | GDT32Bit | 0xF, 0
    };
    gdt[GDTUserCode] = (GDTEntry){
        0xFFFF, 0, 0,
        GDTPresent | GDTUserMode | GDTNotTSS | GDTExecutable | GDTReadWrite,
        GDTPageGranularity | GDT32Bit | 0xF, 0
    };
    gdt[GDTUserData] = (GDTEntry){
        0xFFFF, 0, 0,
        GDTPresent | GDTUserMode | GDTNotTSS | GDTReadWrite,
        GDTPageGranularity | GDT32Bit | 0xF, 0
    };

    // TSS descriptor
    GDTEntry* tssEntry = &gdt[GDTTSS];
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
    tss.ss0 = gdtOffset(GDTKernelData);
    tss.esp0 = getESP(); // Kernel stack to use -- this will get replaced upon context switch
    tss.iopb = tssSize; // Apparently this value works just fine

    // Let's go
    gdtDesc.sizeMinusOne = sizeof(gdt) - 1;
    gdtDesc.offset = gdt;
    loadGDT(&gdtDesc);
}
