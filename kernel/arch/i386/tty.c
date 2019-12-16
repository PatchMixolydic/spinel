#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include <kernel/tty.h>

#include "io.h"
#include "memory/paging.h"
#include "vga.h"

static const size_t VGAWidth = 80;
static const size_t VGAHeight = 25;
static const uint16_t VGACommandPort = 0x3D4;
static const uint16_t VGADataPort = 0x3D5;
static const uint16_t VGACursorStartCommand = 0x0A;
static const uint16_t VGACursorEndCommand = 0x0B;
static const uint16_t VGACursorHighCommand = 0x0E;
static const uint16_t VGACursorLowCommand = 0x0F;
static const uint16_t VGACursorDisableData = 0x20;
static uint16_t* const VGAMemory = (uint16_t*)(KernelOffset + 0x000B8000);

static size_t terminalX;
static size_t terminalY;
static uint8_t terminalColour;
static uint16_t* terminalBuffer;

void terminalInitialize() {
    terminalX = 0;
    terminalY = 0;
    terminalSetColour(vgaCharacterColour(VGAWhite, VGABlack));
    terminalBuffer = VGAMemory;
    terminalDisableCursor();
}

void terminalClear() {
    terminalMoveCursor(0, 0);
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
    if (c == '\n') {
        terminalX = 0;
        terminalY++;
    } else if (c == '\t') {
        terminalX = (terminalX / 4 + 1) * 4;
    } else {
	    terminalPutAt(c, terminalColour, terminalX, terminalY);
        terminalX++;
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
    for (size_t i = 0; i < size; i++) {
        terminalPutChar(data[i]);
    }
    terminalMoveCursor(terminalX, terminalY);
}

void terminalWriteString(const char* data) {
    terminalWrite(data, strlen(data));
}

void terminalEnableCursor(uint8_t startLine, uint8_t endLine) {
    outb(VGACommandPort, VGACursorStartCommand);
    outb(VGADataPort, (inb(VGADataPort) & 0xC0) | startLine);
    outb(VGACommandPort, VGACursorEndCommand);
    outb(VGADataPort, (inb(VGADataPort) & 0xE0) | endLine);
}

void terminalDisableCursor() {
    outb(VGACommandPort, VGACursorStartCommand);
    outb(VGADataPort, VGACursorDisableData);
}

void terminalMoveCursor(size_t x, size_t y) {
    uint16_t newPos = y * VGAWidth + x;
    terminalX = x;
    terminalY = y;
    outb(VGACommandPort, VGACursorLowCommand);
    outb(VGADataPort, newPos & 0xFF);
    outb(VGACommandPort, VGACursorHighCommand);
    outb(VGADataPort, (newPos >> 8) & 0xFF);
}
