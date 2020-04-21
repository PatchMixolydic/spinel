#ifndef SPINEL_TIMER_H
#define SPINEL_TIMER_H

#include <stdint.h>

void timerSetFrequency(uint64_t newFrequency);
void timerTick(void);
uint64_t timerGetTicks(void);

#endif // ndef SPINEL_TIMER_H
