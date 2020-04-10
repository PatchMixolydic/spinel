#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <spinel/cpu.h>
#include "cpu.h"

typedef struct StackBase {
    struct StackBase* ebp;
    uintptr_t eip;
} StackBase;

void printBacktrace(void) {
    printf("Backtrace:\n");
    for (StackBase* base = getEBP(); base != NULL; base = base->ebp) {
        printf("    0x%X\n", base->eip);
    }
}
