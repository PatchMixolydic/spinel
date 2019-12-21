#include <string.h>
#include <kernel/process.h>
#include <kernel/memory.h>
#include "cpu.h"
#include "gdt.h"
#include "memory/paging.h"

void archProcessSetup(Process* process, uintptr_t codeLoc) {
    if (process == NULL) {
        return;
    }
    printf("Setting up process\n");
    x86State* state = kernelMalloc(sizeof(x86State));
    memset(state, 0, sizeof(x86State));
}
