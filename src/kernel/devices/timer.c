#include <stdint.h>
#include <stdio.h>
#include <spinel/timer.h>

// TODO: should there be some sort of timer registration system?
// It seems better to just let one timer handle everything

// Ticks are in hundredths of a nanosecond

static const uint64_t TicksPerSecond = 100000000000;

static uint64_t ticksSinceBoot = 0;
static uint64_t ticksPerCycle = 0;

// Convert a given frequency to ticks per cycle
// Note: this doesn't work if the clock period is less than 1/100 ns
static inline uint64_t frequencyToTicks(uint64_t freq) {
    return TicksPerSecond / freq;
}

void timerSetFrequency(uint64_t newFrequency) {
    ticksPerCycle = frequencyToTicks(newFrequency);
}

void timerTick(void) {
    ticksSinceBoot += ticksPerCycle;
    if (ticksSinceBoot == 0) {
        printf("ticksSinceBoot overflowed\n");
    }
}

uint64_t timerGetTicks(void) {
    return ticksSinceBoot;
}
