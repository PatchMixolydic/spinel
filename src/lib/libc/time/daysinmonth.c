#include <time.h>

static const int DaysPerMonth[13] = {
    0,
    31, 28, 31, 30, 31, 30, 31,
    31, 30, 31, 30, 31
};

int daysinmonth(int month, int year) {
    if (month == 2) {
        return isleapyear(year) ? 29 : 28;
    } else if (1 <= month && month <= 12) {
        return DaysPerMonth[month];
    } else {
        return 0;
    }
}
