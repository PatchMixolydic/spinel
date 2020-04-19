#ifndef SPINEL_MULTITASKING_H
#define SPINEL_MULTITASKING_H

#include <sys/types.h>

struct ArchTask;

typedef struct {
    pid_t pid;
    char* name;
    uid_t effectiveUser;
    uid_t user;
} Process;

typedef struct {
    struct ArchTask* task;
    pid_t tid;
    pid_t pid;
} Thread;

// Architecture specific task switch code
// Takes the arch task and the tid
// Presumably, the tid is only used for debugging purposes,
// like flagging that a task provided weird values in its
// ArchTask
int archTaskSwitch(struct ArchTask* task, pid_t tid);

#endif // ndef SPINEL_MULTITASKING_H
