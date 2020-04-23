#ifndef SPINEL_TIMER_H
#define SPINEL_TIMER_H

#include <stdbool.h>
#include <stdint.h>

extern const uint64_t TicksPerSecond;

typedef void (*TimerAlarmCallback)(void);

void timerSetFrequency(uint64_t newFrequency);
// Set an alarm to go off. After `time` ticks, `callback` will be
// called. If `oneShot` is set, the alarm will only go off once.
// Othewise, it will go off every `time` ticks.
// Returns the timer ID, which can be used to remove the alarm.
uint64_t timerSetAlarm(
    uint64_t interval, bool oneShot, TimerAlarmCallback callback
);
void timerRemoveAlarm(uint64_t id);
void timerTick(void);
uint64_t timerGetTicksSinceBoot(void);

#endif // ndef SPINEL_TIMER_H
