#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <time.h>
#include <units.h>
#include <spinel/clock.h>
#include <spinel/timer.h>

// Time in seconds between resyncs of the clock
static const int ResyncTime = 1 * Hour;

static ClockSource* clock = NULL;
static ClockTime* currentTime = NULL;

static void clockRefreshTime(void) {
    if (clock == NULL || clock->getTime == NULL) {
        return;
    }

    currentTime = clock->getTime();
}

static void secondPassed(void) {
    if (time == NULL) {
        return;
    }

    currentTime->second++;
    // TODO: leap seconds?
    if (currentTime->second > 59) {
        currentTime->second = 0;
        currentTime->minute++;
    }
    if(currentTime->minute > 59) {
        currentTime->minute = 0;
        currentTime->hour++;
    }
    if(currentTime->hour > 23) {
        currentTime->hour = 0;
        currentTime->day++;
    }
    if(currentTime->day > daysinmonth(currentTime->month, currentTime->year)) {
        currentTime->day = 1;
        currentTime->month++;
    }
    if(currentTime->month > 12) {
        currentTime->month = 1;
        currentTime->year++;
    }
}

void clockCreateAlarms(void) {
    // This can't be done at clock source registration because alloc will probably
    // not be initialized
    timerSetAlarm(ResyncTime * TicksPerSecond, false, clockRefreshTime);
    timerSetAlarm(1 * TicksPerSecond, false, secondPassed);
}

void registerClockSource(ClockSource* newClock) {
    if (clock != NULL) {
        printf("Tried to register a clock, but one is already registered\n");
        return;
    } else if (newClock == NULL) {
        return;
    } else if (newClock->getTime == NULL) {
        printf("Tried to register a new clock without a getTime function\n");
        return;
    }

    clock = newClock;
    clockRefreshTime();
}

ClockTime* clockGetTime(void) {
    return currentTime;
}

bool clockTimesEqual(ClockTime* a, ClockTime* b) {
    if (a == NULL || b == NULL) {
        // NULL, NULL aren't equal times, just equal pointers
        return false;
    }

    return a->year == b->year &&\
        a->month == b->month &&\
        a->day == b->day &&\
        a->hour == b->hour &&\
        a->minute == b->minute &&\
        a->second == b->second;
}
