#ifndef SPINEL_CLOCK_H
#define SPINEL_CLOCK_H

#include <stdbool.h>

typedef struct {
    int year;
    int month;
    int day;
    int hour;
    int minute;
    int second;
} ClockTime;

typedef ClockTime* (*ClockGetTimeFunc)(void);

typedef struct {
    ClockGetTimeFunc getTime;
} ClockSource;

void registerClockSource(ClockSource* newClock);
ClockTime* clockGetTime(void);
bool clockTimesEqual(ClockTime* a, ClockTime* b);

#endif // ndef SPINEL_CLOCK_H