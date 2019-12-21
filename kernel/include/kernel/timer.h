#ifndef _KERNEL_TIMER_H
#define _KERNEL_TIMER_H

typedef unsigned long long TickCount;

void setupTimer(int frequency, bool oneShot);
void enableTimer();
void disableTimer();
bool isTimerEnabled();
TickCount getTickCount();
void resetTickCount();

#endif // nfdef _KERNEL_TIMER_H
