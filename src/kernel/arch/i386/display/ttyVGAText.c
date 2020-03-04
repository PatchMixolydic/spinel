#include <stdbool.h>
#include <stddef.h>
#include <hakurei.h>
#include <spinel/kernelInfo.h>
#include <spinel/tty.h>
#include "../core/cpu.h"

typedef uint16_t VGAChar;

typedef enum {
    VGABlack,
    VGABlue,
    VGAGreen,
    VGACyan,
    VGARed,
    VGAMagenta,
    VGABrown,
    VGALightGrey
} VGAColour;

static const VGAColour VGAToANSIColour[] = {
    VGABlack,
    VGARed,
    VGAGreen,
    VGABrown,
    VGABlue,
    VGAMagenta,
    VGACyan,
    VGALightGrey
};

static const uint8_t VGABrightBit = 0x8; // 0b1000
static const unsigned int VGAWidth = 80;
static const unsigned int VGAHeight = 25;
static const uint16_t VGACursorCommandPort = 0x3D4;
static const uint16_t VGACursorDataPort = 0x3D5;

static VGAChar* textBuffer = (VGAChar*)(0xC00B8000);

static void putCharCallback(char c);
static void updateCursorCallback(void);
static void belowScreenCallback(void);
static void clearScreenCallback(void);

static HakuTerminalState state = {
    VGAWidth, VGAHeight,
    0, 0,
    true, false, true, false,
    HakuANSILightGrey, HakuANSIBlack,
    HakuANSILightGrey, HakuANSIBlack,
    HakuNoError,
    putCharCallback,
    updateCursorCallback,
    belowScreenCallback,
    clearScreenCallback
};

// Convert char to VGA char, using current colour
static inline VGAChar toVGAChar(char c) {
    VGAColour bg =
        VGAToANSIColour[state.bgColour] + (state.bgBright ? VGABrightBit : 0);
    VGAColour fg =
        VGAToANSIColour[state.fgColour] + (state.fgBright ? VGABrightBit : 0);
    VGAChar colour = (bg << 4) | fg;
    return (uint16_t)(colour << 8) | c;
}

static void putCharCallback(char c) {
    textBuffer[state.y * VGAWidth + state.x] = toVGAChar(c);
}

static void updateCursorCallback(void) {
    uint16_t newPos = state.y * VGAWidth + state.x;
    outByte(VGACursorCommandPort, 0x0F);
    outByte(VGACursorDataPort, (uint8_t)(newPos & 0xFF));
    outByte(VGACursorCommandPort, 0x0E);
    outByte(VGACursorDataPort, (uint8_t)(newPos >> 8));
}

static void belowScreenCallback(void) {
    // Scrolling time!
    for (unsigned int i = 0; i < VGAWidth * VGAHeight; i++) {
        if (i < VGAWidth * (VGAHeight - 1)) {
            // Take the character from the next line
            textBuffer[i] = textBuffer[i + VGAWidth];
        } else {
            // Clear the bottom line
            textBuffer[i] = toVGAChar(' ');
        }
    }
}

static void clearScreenCallback(void) {
    for (unsigned int i = 0; i < VGAWidth * VGAHeight; i++) {
        textBuffer[i] = toVGAChar(' ');
    }
}

void enableCursor(void) {
    outByte(VGACursorCommandPort, 0x0A);
    outByte(VGACursorDataPort, (inByte(VGACursorDataPort) & 0xC0) | 0xE);
    outByte(VGACursorCommandPort, 0x0B);
    outByte(VGACursorDataPort, (inByte(VGACursorDataPort) & 0xC0) | 0xF);
}

void disableCursor(void) {
    outByte(VGACursorCommandPort, 0x0A);
    outByte(VGACursorDataPort, 0x20);
}

void putChar(char c) {
    hakuPutChar(&state, c);
}

void putString(const char s[]) {
    hakuPutString(&state, s);
}

void putStringLen(const char s[], size_t length) {
    hakuPutStringLen(&state, s, length);
}

void clearScreen(void) {
    hakuClearScreen(&state);
}
