#ifndef _KERNEL_SCHEDULER_H
#define _KERNEL_SCHEDULER_H

#include <kernel/process.h>

void initScheduler();
void createIdleProcess();
void scheduleNext();
void schedulerCheckIn();
Process* getCurrentProcess();

#endif // ndef _KERNEL_SCHEDULER_H
