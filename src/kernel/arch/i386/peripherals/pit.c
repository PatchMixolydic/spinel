#include <assert.h>
#include <stdint.h>
#include <spinel/timer.h>
#include "../core/cpu.h"
#include "../interrupts/interrupts.h"
#include "pic.h"

static const unsigned BaseFrequency = 1193182;
static const unsigned Divider = 500;
static const unsigned EffectiveFrequency = BaseFrequency / Divider;

static const uint16_t Channel0Port = 0x40;
static const uint16_t PITCommandPort = 0x43;

// Mode/command port values
static const uint8_t OperatingModeOffset = 1;
static const uint8_t AccessModeOffset = 4;
static const uint8_t ChannelSelectOffset = 6;
// Operation modes
static const uint8_t RateGeneratorMode = 2;
static const uint8_t SquareWaveMode = 3;
// Access modes
static const uint8_t LatchCountMode = 0;
// Only the low byte needs to be written for the reload value
static const uint8_t LowByteMode = 1;
static const uint8_t HighByteMode = 2;
static const uint8_t LowHighByteMode = 3;

static void setupChannel(uint8_t channel, uint8_t accessMode, uint8_t opMode) {
    assert(channel == 0 || channel == 2);
    outByte(
        PITCommandPort,
        (channel << ChannelSelectOffset) |
        (accessMode << AccessModeOffset) |
        (opMode << OperatingModeOffset)
    );
}

void initPIT(void) {
    // Clear current reload value
    outByte(Channel0Port, 0);
    outByte(Channel0Port, 0);
    setupChannel(0, LowHighByteMode, RateGeneratorMode);
    // Load channel 0 with frequency divider
    outByte(Channel0Port, Divider & 0xFF);
    outByte(Channel0Port, (Divider >> 8) & 0xFF);
    timerSetFrequency(EffectiveFrequency);
    registerInterruptHandler(IntIRQ0, timerTick);
    picSetIRQMasked(0, false);
}
