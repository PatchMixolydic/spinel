#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <sys/types.h>
#include <spinel/cpu.h>
#include <spinel/kernelInfo.h>
#include "archMultitasking.h"

ArchTask* currentArchTask = NULL;

int archTaskSwitch(ArchTask* task, pid_t tid) {
    // Basic validation
    printf("Task switch to %u\n", tid);
    if (
        task == NULL || task->esp == NULL ||
        task->kernelESP < (void*)KernelOffset
    ) {
        if (task != NULL) {
            // TODO: length specifier
            printf("Suspicious! Invalid x86 task info for thread %u\n", tid);
            printf(
                "esp = 0x%X, kernelESP = 0x%X, cr3=0x%X\n",
                task->esp, task->kernelESP, task->cr3
            );
        }
        return -EFAULT;
    }
    printf("0x%X passes check\n", task);
    disableInterrupts();
    x86TaskSwitch(task);
    enableInterrupts();
    printf("Switched to %u\n", tid);

    return 0;
}
