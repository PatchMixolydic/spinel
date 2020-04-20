#include <stdio.h>
#include <sys/types.h>

#ifdef __Kernel
#include <spinel/clock.h>
#endif

static const time_t UnixSecondsPerDay = 86400;
static const time_t UnixSecondsPerHour = 3600;
static const time_t UnixSecondsPerMinute = 60;
static const time_t UnixEpochYear = 1970;
static const time_t DaysPerMonth[13] = {
    0,
    31, 28, 31, 30, 31, 30, 31,
    31, 30, 31, 30, 31
};

static inline bool isLeapYear(int year) {
    return year % 4 == 0 && (year % 100 != 0 || year % 400 == 0);
}

static inline time_t getDaysForMonth(int month, int year) {
    if (month == 2) {
        return isLeapYear(year) ? 29 : 28;
    } else if (1 <= month && month <= 12) {
        return DaysPerMonth[month];
    } else {
        printf("Invalid month %d\n", month);
        return 0;
    }
}

static inline time_t getDaysForYear(int year) {
    return isLeapYear(year) ? 366 : 365;
}

time_t time(time_t* dest) {
    time_t res;
    #ifdef __Kernel
        ClockTime* clockTime = clockGetTime();
        if (clockTime == NULL) {
            res = -1;
            if (dest != NULL) {
                *dest = res;
            }
            return res;
        }

        res = 0;
        if (clockTime->year > UnixEpochYear) {
            for (int year = clockTime->year; year > UnixEpochYear; year--) {
                res += getDaysForYear(year) * UnixSecondsPerDay;
            }
        } else if (clockTime->year < UnixEpochYear) {
            for (int year = clockTime->year; year < UnixEpochYear; year++) {
                res -= getDaysForYear(year) * UnixSecondsPerDay;
            }
        }

        for (int month = clockTime->month - 1; month >= 1; month--) {
            res += getDaysForMonth(month, clockTime->year) * UnixSecondsPerDay;
        }

        res += UnixSecondsPerDay * (clockTime->day - 2);
        res += UnixSecondsPerHour * clockTime->hour;
        res += UnixSecondsPerMinute * (clockTime->minute - 1);
        res += clockTime->second;
    #else
        printf("Warning: time() is not implemented for userspace");
        res = -1;
    #endif

    if (dest != NULL) {
        *dest = res;
    }
    return res;
}
