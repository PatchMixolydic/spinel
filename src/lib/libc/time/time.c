#include <stdio.h>
#include <sys/types.h>
#include <time.h>
#include <units.h>

#ifdef __Kernel
#include <spinel/clock.h>
#endif

static const time_t UnixEpochYear = 1970;

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
                res += daysinyear(year) * Day;
            }
        } else if (clockTime->year < UnixEpochYear) {
            for (int year = clockTime->year; year < UnixEpochYear; year++) {
                res -= daysinyear(year) * Day;
            }
        }

        for (int month = clockTime->month - 1; month >= 1; month--) {
            res += daysinmonth(month, clockTime->year) * Day;
        }

        res += Day * (clockTime->day - 2);
        res += Hour * clockTime->hour;
        res += Minute * (clockTime->minute - 1);
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
