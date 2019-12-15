#include <stdint.h>
#include "paging.h"

typedef struct {
    uint16_t limitLow;
    uint16_t baseLow;
    uint8_t baseMid;
    uint8_t access;
    uint8_t limitHigh : 4;
    uint8_t flags : 4;
    uint8_t baseHigh;
} __attribute__((packed)) gdtEntry;

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
} __attribute__((packed)) taskStateSegment;
