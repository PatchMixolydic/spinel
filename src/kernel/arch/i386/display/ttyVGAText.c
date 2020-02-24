#include <stddef.h>
#include <spinel/tty.h>
#include "../core/kernel.h"

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

static vgachar_t* const textBuffer = (vgachar_t*)(KernelOffset + 0x000B8000);
static unsigned int terminalX = 0, terminalY = 0;
static uint8_t terminalColour = 0x0F;

// Convert char to VGA char, using current colour
static inline vgachar_t toVGAChar(char c) {
    return ((uint16_t)(terminalColour) << 8) | c;
}

static void newline() {
    terminalX = 0;
    terminalY++;
    if (terminalY >= VGAHeight) {
        // Scrolling time!
        // TODO: memcpy
        for (int i = 0; i < VGAWidth * (VGAHeight - 1); i++) {
            // Take the character from the next line
            textBuffer[i] = textBuffer[i + VGAWidth];
        }
        for (
            int i = VGAWidth * (VGAHeight - 1);
            i < VGAWidth * VGAHeight;
            i++
        ) {
            // Clear the bottom line
            textBuffer[i] = toVGAChar(' ');
        }
        terminalY = VGAHeight - 1;
    }
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

void putString(char s[]) {
    for (int i = 0; s[i] != '\0'; i++) {
        putChar(s[i]);
    }
}
