#ifndef ARCH_I386_PIT_H
#define ARCH_I386_PIT_H

typedef enum {
    PITInterruptOnCount = 0,
    PITHardwareOneShot,
    PITRateGenerator,
    PITSquareWave,
    PITSoftwareStrobe,
    PITHardwareStrobe
} PITMode;

#endif // ndef ARCH_I386_PIT_H
