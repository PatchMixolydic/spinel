#include <stdint.h>
#include "pit.h"
#include "io.h"

const int PITChannel0DataPort = 0x40;
const int PITChannel1DataPort = 0x41;
const int PITChannel2DataPort = 0x42;
const int PITCommandRegister = 0x43;

const int PITLatchCount = 0x00 << 4;
const int PITAccessLowByte = 0x01 << 4;
const int PITAccessHighByte = 0x02 << 4;
const int PITAccessLowHighByte = 0x03 << 4;
const int PITBCDMode = 1;

const int PITMasterClock = 1193182;

static inline uint16_t getReloadValue(int frequency) {
    // TODO: round math.h
    uint16_t res = PITMasterClock / frequency;
    if ((float)PITMasterClock / frequency - res >= 0.5) {
        return res + 1;
    }
    return res;
}

void setupPIC(int channel, PITMode mode, int frequency) {
    uint16_t reloadVal = getReloadValue(frequency);
    uint8_t command = channel << 6 | PITAccessLowHighByte | mode << 1;
    uint8_t dataPort = PITChannel0DataPort + channel;
    outByte(PITCommandRegister, command);
    outByte(dataPort, reloadVal); // send low byte
    outByte(dataPort, reloadVal >> 8); // send high byte
}

void setupTimer(int frequency, bool oneShot) {
    setupPIC(0, oneShot ? PITHardwareOneShot : PITSquareWave, frequency);
}
