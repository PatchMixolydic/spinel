#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <spinel/cpu.h>
#include <spinel/arch/i386/cpu.h>

typedef struct StackBase {
    struct StackBase* ebp;
    uintptr_t eip;
} StackBase;

void printBacktrace(void) {
    printf("Backtrace:\n");
    for (
        StackBase* base = (StackBase*)getEBP(); base != NULL; base = base->ebp
    ) {
        printf("    0x%X\n", base->eip);
    }
}
