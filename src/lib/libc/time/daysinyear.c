#include <time.h>

int daysinyear(int year) {
    return isleapyear(year) ? 366 : 365;
}
