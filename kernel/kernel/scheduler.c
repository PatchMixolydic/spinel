#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <kernel/cpu.h>
#include <dataStructures/doublyLinkedList.h>
#include <kernel/process.h>
#include <kernel/timer.h>

static const int TicksPerSecond = 10000; // 1/10 ms
static const int TimeQuantum = 200; // ticks

static DoublyLinkedList queue = {NULL, NULL};
static DoublyLinkedList sleepQueue = {NULL, NULL};
static Process* idleProcess = NULL;
static Process* currentProcess = NULL;

void initScheduler() {
    createIdleProcess();
    setupTimer(TicksPerSecond, false);
    enableTimer();
    printf("Scheduler coming online\n");
}

void idle() {
    printf("Idle process active\n");
    while (true) {
        haltCPU(); // wait for interrupts
    }
}

void createIdleProcess() {
    printf("Create idle process\n");
    if (idleProcess != NULL) {
        printf("Tried to create idle process twice :(\n");
        return;
    }
    idleProcess = spawnSystemProcess("[idle]", (uintptr_t)idle);
}

void scheduleNext() {
    printf("Scheduling\n");
    if (queue.first != NULL) {
        Process* potentialProcess = queue.first->data;
        if (potentialProcess != NULL) {
            while (potentialProcess != NULL && potentialProcess->finished) {
                // This is finished, we shouldn't schedule this!
                dllistPopFront(&queue);
                if (queue.first == NULL) {
                    potentialProcess = NULL;
                    break;
                }
                potentialProcess = queue.first->data;
            }
            if (potentialProcess != NULL) {
                // Looks good to me
                switchFromProcess(currentProcess);
                currentProcess = potentialProcess;
                switchToProcess(currentProcess);
                return;
            }
        }
    }
    // If we're here, potentialProcess is NULL... time for the idle process!
    if (idleProcess == NULL) {
        createIdleProcess();
    }
    switchFromProcess(currentProcess);
    currentProcess = idleProcess;
    switchToProcess(currentProcess);
    return;
}

void schedulerCheckIn() {
    if (currentProcess != NULL) {
        if (getTickCount() - currentProcess->timesliceStart >= TimeQuantum) {
            printf("Process timeslice expired\n");
            // You're all out of time, I'm afraid!
            dllistAddBack(&queue, dllistPopFront(&queue));
            // Schedule the next process
            scheduleNext();
        } else if (currentProcess->timesliceStart > getTickCount()) {
            // ... oh. It overflowed... so much for an unsigned long long
            currentProcess->timesliceStart = 0;
        }
    } else {
        // Hmm... maybe if I...
        scheduleNext();
    }
}

Process* getCurrentProcess() {
    return currentProcess;
}
