#include <string.h>
#include <kernel/memory.h>
#include <kernel/process.h>
#include <kernel/timer.h>

static ProcessID nextProcessID = 1;

Process* spawnSystemProcess(const char* name, uintptr_t codeLoc) {
    Process* res = kernelMalloc(sizeof(Process));
    memset(res, 0, sizeof(Process));
    res->name = name;
    archProcessSetup(res, codeLoc);
    return res;
}

Process* spawnProcess(const char* name, uintptr_t codeLoc) {
    Process* res = spawnProcess(name, codeLoc);
    res->id = nextProcessID++;
    return res;
}

void switchToProcess(Process* process) {
    if (process == NULL) {
        return;
    }
    printf("Switching to process 0x%X\n", process);
    process->timesliceStart = getTickCount();
}

void switchFromProcess(Process* process) {
    if (process == NULL) {
        return;
    }
    printf("Switching from process 0x%X\n", process);
}
