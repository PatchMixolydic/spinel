#ifndef ARCH_I386_PIT_H
#define ARCH_I386_PIT_H

#include <kernel/timer.h>

typedef enum {
    PITInterruptOnCount = 0,
    PITHardwareOneShot,
    PITRateGenerator,
    PITSquareWave,
    PITSoftwareStrobe,
    PITHardwareStrobe
} PITMode;

static TickCount tickCount = 0;

void setupPIC(int channel, PITMode mode, int frequency);

#endif // ndef ARCH_I386_PIT_H
