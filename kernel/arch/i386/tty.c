#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include <kernel/tty.h>

#include "vga.h"

static const size_t VGAWidth = 80;
static const size_t VGAHeight = 25;
static uint16_t* const VGAMemory = (uint16_t*) 0x000B8000;

static size_t terminalX;
static size_t terminalY;
static uint8_t terminalColour;
static uint16_t* terminalBuffer;

void terminalInitialize(void) {
    terminalX = 0;
    terminalY = 0;
    terminalSetColour(vgaCharacterColour(VGAWhite, VGABlack));
    terminalBuffer = VGAMemory;
}

void terminalClear(void) {
    for (size_t y = 0; y < VGAHeight; y++) {
        for (size_t x = 0; x < VGAWidth; x++) {
            const size_t index = y * VGAWidth + x;
            terminalBuffer[index] = vgaCharacter(' ', terminalColour);
        }
    }
}

void terminalSetColour(uint8_t color) {
    terminalColour = color;
}

void terminalPutAt(unsigned char c, uint8_t color, size_t x, size_t y) {
    const size_t index = y * VGAWidth + x;
    terminalBuffer[index] = vgaCharacter(c, color);
}

void terminalPutChar(char c) {
    if (c != '\n') {
	    terminalPutAt(c, terminalColour, terminalX, terminalY);
        terminalX++;
    } else {
        terminalX = 0;
        terminalY++;
    }
	if (terminalX >= VGAWidth) {
		terminalX = 0;
        terminalY++;
	}
    if (terminalY >= VGAHeight) {
        // scrolling time
        for (int i = 0; i < VGAWidth * VGAHeight; i++) {
            if (i < (VGAHeight - 1) * VGAWidth) {
                terminalBuffer[i] = terminalBuffer[i + VGAWidth];
            } else {
                terminalBuffer[i] = vgaCharacter(' ', terminalColour);
            }
        }
        terminalY = VGAHeight - 1;
    }
}

void terminalWrite(const char* data, size_t size) {
    for (size_t i = 0; i < size; i++)
        terminalPutChar(data[i]);
}

void terminalWriteString(const char* data) {
    terminalWrite(data, strlen(data));
}
