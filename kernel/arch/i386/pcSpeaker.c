#include <stdbool.h>
#include "io.h"

const int PCSpeakerPort = 0x61;

// The PC speaker is mostly tied to PIT channel 2, so this is pretty empty...

void setSpeakerState(bool state) {
    if (state) {
        outByte(0x61, inByte(0x61) | 0x03);
    } else {
        outByte(0x61, inByte(0x61) & ~0x03);
    }
}
