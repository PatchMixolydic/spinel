#include <stddef.h>
#include <spinel/ansi.h>
#include <spinel/kernelInfo.h>
#include <spinel/tty.h>
#include "../core/cpu.h"

typedef uint16_t vgachar_t;

typedef enum {
    VGABlack,
    VGABlue,
    VGAGreen,
    VGACyan,
    VGARed,
    VGAMagenta,
    VGABrown,
    VGALightGrey
} VGAColours;

static const uint8_t VGABrightBit = 0x80; // 0b1000_0000
static const unsigned int VGAWidth = 80;
static const unsigned int VGAHeight = 25;
static const uint16_t VGACursorCommandPort = 0x3D4;
static const uint16_t VGACursorDataPort = 0x3D5;

static vgachar_t* textBuffer = (vgachar_t*)(0xC00B8000);
static unsigned int terminalX = 0, terminalY = 0;
static uint8_t terminalColour = 0x0F;

static inline void setTerminalColour(uint8_t fg, uint8_t bg) {
    terminalColour = (fg << 4) | bg;
}

// Convert char to VGA char, using current colour
static inline vgachar_t toVGAChar(char c) {
    return ((uint16_t)(terminalColour) << 8) | c;
}

static void newline(void) {
    terminalX = 0;
    terminalY++;
    if (terminalY >= VGAHeight) {
        // Scrolling time!
        for (unsigned int i = 0; i < VGAWidth * VGAHeight; i++) {
            if (i < VGAWidth* (VGAHeight - 1)) {
                // Take the character from the next line
                textBuffer[i] = textBuffer[i + VGAWidth];
            } else {
                // Clear the bottom line
                textBuffer[i] = toVGAChar(' ');
            }
        }
        terminalY = VGAHeight - 1;
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

void updateCursor(void) {
    uint16_t newPos = terminalY * VGAWidth + terminalX;
    outByte(VGACursorCommandPort, 0x0F);
    outByte(VGACursorDataPort, (uint8_t)(newPos & 0xFF));
    outByte(VGACursorCommandPort, 0x0E);
    outByte(VGACursorDataPort, (uint8_t)(newPos >> 8));
}

// Put a character at the current position
void putChar(char c) {
    switch (c) {
        case '\n':
            newline();
            break;
        default: {
            textBuffer[terminalY * VGAWidth + terminalX] = toVGAChar(c);
            terminalX++;
            if (terminalX >= VGAWidth) {
                newline();
            }
            break;
        }
    }
}

void putString(const char s[]) {
    for (int i = 0; s[i] != '\0'; i++) {
        if (s[i] == '\x1B') {
            i += parseANSIEscape(s + i);
        } else {
            putChar(s[i]);
        }
    }
    updateCursor();
}

void putStringLen(const char s[], size_t length) {
    for (size_t i = 0; i < length; i++) {
        if (s[i] == '\x1B') {
            i += parseANSIEscape(s + i);
        } else {
            putChar(s[i]);
        }
    }
    updateCursor();
}

void clearScreen(void) {
    for (unsigned int i = 0; i < VGAWidth * VGAHeight; i++) {
        textBuffer[i] = toVGAChar(' ');
    }
    terminalX = 0;
    terminalY = 0;
    updateCursor();
}
