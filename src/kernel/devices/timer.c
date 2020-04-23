#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <dastr/linkedList.h>
#include <spinel/timer.h>

// TODO: should there be some sort of timer registration system?
// It seems better to just let one timer handle everything

typedef struct {
    uint64_t id;
    uint64_t interval;
    uint64_t ticks;
    bool oneShot;
    TimerAlarmCallback callback;
} Alarm;

// Ticks are in hundredths of a nanosecond
const uint64_t TicksPerSecond = 100000000000;

static uint64_t nextTimerId = 1;
static uint64_t ticksSinceBoot = 0;
static uint64_t ticksPerCycle = 0;
static LinkedList* alarms = NULL;

// Convert a given frequency to ticks per cycle
// Note: this doesn't work if the clock period is less than 1/100 ns
static inline uint64_t frequencyToTicks(uint64_t freq) {
    return TicksPerSecond / freq;
}

void timerSetFrequency(uint64_t newFrequency) {
    ticksPerCycle = frequencyToTicks(newFrequency);
}

uint64_t timerSetAlarm(
    uint64_t interval, bool oneShot, TimerAlarmCallback callback
) {
    if (alarms == NULL) {
        alarms = linkedListCreate();
    }
    assert(callback);

    Alarm* newAlarm = malloc(sizeof(Alarm));
    // TODO: think of a better way to handle ids
    if (nextTimerId == 0) {
        nextTimerId++;
    }
    newAlarm->id = nextTimerId++;
    newAlarm->interval = interval;
    newAlarm->oneShot = oneShot;
    newAlarm->callback = callback;
    newAlarm->ticks = 0;
    linkedListInsertLast(alarms, newAlarm);
    return newAlarm->id;
}

void timerRemoveAlarm(uint64_t id) {
    if (alarms == NULL) {
        printf("Tried to remove alarm when none were registered\n");
        return;
    }

    ForEachInList(alarms, alarmNode) {
        Alarm* alarm = alarmNode->data;
        assert(alarm);
        if (id != alarm->id) {
            continue;
        }

        linkedListRemoveNode(alarms, alarmNode);
        return;
    }

    printf("Tried to remove timer %u, but it wasn't registered!\n", id);
}

void timerTick(void) {
    ticksSinceBoot += ticksPerCycle;
    if (ticksSinceBoot == 0) {
        printf("ticksSinceBoot overflowed\n");
    }

    if (alarms != NULL) {
        ForEachInList(alarms, alarmNode) {
            Alarm* alarm = alarmNode->data;
            assert(alarm);
            alarm->ticks += ticksPerCycle;
            while (alarm->ticks >= alarm->interval) {
                alarm->callback();
                alarm->ticks -= alarm->interval;
                if (alarm->oneShot) {
                    timerRemoveAlarm(alarm->id);
                    break;
                }
            }
        }
    }
}

uint64_t timerGetTicksSinceBoot(void) {
    return ticksSinceBoot;
}
