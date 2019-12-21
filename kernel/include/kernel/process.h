#ifndef _KERNEL_PROCESS_H
#define _KERNEL_PROCESS_H

#include <stdbool.h>
#include <stdint.h>
#include <kernel/timer.h>

typedef uint64_t ProcessID;
typedef struct {
    ProcessID id;
    char* name;
    char* commandLine;
    uint32_t userID; // TODO: user id type
    TickCount timesliceStart; // TODO: timestamp
    bool finished;
    void* archState; // arch-specific state
} Process;

// Spawns a process from nothing.
// Mostly useful for init and the idle task.
Process* spawnSystemProcess(const char* name, uintptr_t codeLoc);
// Currently also spawns a process from nothing.
// Stopgap because there is no process hierarchy
Process* spawnProcess(const char* name, uintptr_t codeLoc);
// Architecture dependent process setup. This is probably mostly
// setting up archState.
void archProcessSetup(Process* process, uintptr_t codeLoc);
void switchToProcess(Process* process);
void switchFromProcess(Process* process);

#endif // ndef _KERNEL_PROCESS_H
