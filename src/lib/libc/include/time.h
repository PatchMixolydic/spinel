#ifndef _LIBC_TIME_H
#define _LIBC_TIME_H

#include <sys/types.h>

time_t time(time_t* dest);

// Spinel extensions
// Mostly convenience functions for time-related business
// Returns the number of days in the given month during the given year,
// or 0 if an invalid month is passed
int daysinmonth(int month, int year);
// Returns 365 or 366 days, depending on whether or
// not the provided year is a leap year
int daysinyear(int year);
// Returns if the given year is a leap year. 
// This should return a bool, but C89 forbids it
int isleapyear(int year);

#endif // ndef _LIBC_TIME_H
