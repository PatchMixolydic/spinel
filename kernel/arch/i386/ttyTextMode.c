#include <ctype.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <kernel/tty.h>
#include "io.h"
#include "memory/paging.h"

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

typedef enum VGAColour {
    VGABlack, VGABlue, VGAGreen, VGACyan,
    VGARed, VGAMagenta, VGABrown, VGALightGrey,
    VGADarkGrey, VGALightBlue, VGALightGreen, VGALightCyan,
    VGALightRed, VGALightMagenta, VGAYellow, VGAWhite
} VGAColour;

static const VGAColour ANSICodeToColour[] = {
    VGABlack, VGARed, VGAGreen, VGABrown,
    VGABlue, VGAMagenta, VGACyan, VGALightGrey,
    VGADarkGrey, VGALightRed, VGALightGreen, VGAYellow,
    VGALightBlue, VGALightMagenta, VGALightCyan, VGAWhite
};

static size_t terminalX;
static size_t terminalY;
static uint8_t terminalColour;
static uint16_t* terminalBuffer;

static inline void setTerminalColour(uint8_t colour) {
    terminalColour = colour;
}

static inline void setBGColour(VGAColour colour) {
    setTerminalColour((terminalColour & 0x0F) | (colour << 4));
}

static inline void setFGColour(VGAColour colour) {
    setTerminalColour((terminalColour & 0xF0) | colour);
}

static inline uint8_t vgaCharacterColour(VGAColour fg, VGAColour bg) {
    return bg << 4 | fg;
}

static inline uint16_t vgaCharacter(unsigned char c, uint8_t colour) {
    return (uint16_t) colour << 8 | (uint16_t) c;
}

void terminalInitialize() {
    terminalX = 0;
    terminalY = 0;
    setTerminalColour(vgaCharacterColour(VGAWhite, VGABlack));
    terminalBuffer = VGAMemory;
    terminalDisableCursor();
}

void terminalClear() {
    terminalMoveCursor(0, 0);
    for (size_t i = 0; i < VGAWidth * VGAHeight; i++) {
        terminalBuffer[i] = vgaCharacter(' ', terminalColour);
    }
}

void terminalPutAt(unsigned char c, uint8_t colour, size_t x, size_t y) {
    const size_t index = y * VGAWidth + x;
    terminalBuffer[index] = vgaCharacter(c, colour);
}

void terminalPutChar(char c) {
    switch (c) {
        case '\n':
            terminalX = 0;
            terminalY++;
            break;
        case '\t':
            terminalX = (terminalX / 4 + 1) * 4;
            break;
        default:
            terminalPutAt(c, terminalColour, terminalX, terminalY);
            terminalX++;
            break;
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

// TODO: find a better home for this
static unsigned int stringToInt(const char* start, size_t size) {
    unsigned int res = 0;
    for (int i = 0; i < size; i++) {
        if (!isdigit(start[i])) {
            return 0;
        }
        res *= 10;
        res += (unsigned int)(start[i] - '0');
    }
    return res;
}

static void ansiGraphicRendition(int code) {
    if (code == 0) {
        // Reset
        setTerminalColour(vgaCharacterColour(VGAWhite, VGABlack));
    } else if (30 <= code && code <= 37) {
        // Foreground colour
        setFGColour(ANSICodeToColour[code - 30]);
    } else if (code == 39) {
        // Default foreground colour
        setFGColour(VGAWhite);
    } else if (90 <= code && code <= 97) {
        // Foreground colour, bright
        setFGColour(ANSICodeToColour[code - 90 + 8]);
    } else if (40 <= code && code <= 47) {
        // Background colour
        setBGColour(ANSICodeToColour[code - 40]);
    } else if (code == 49) {
        // Default background colour
        setBGColour(VGABlack);
    } else if (100 <= code && code <= 107) {
        // Background colour, bright
        setBGColour(ANSICodeToColour[code - 100 + 8]);
    }
}

static int processANSIEscape(const char* data, size_t size) {
    int charsToAdvance = 0;
    switch (data[0]) {
        case '[': {
            // Control Sequence introducer
            if (size <= 1) {
                break;
            }
            int cmdIdx = 1;
            for (; cmdIdx < size && !isalpha(data[cmdIdx]); cmdIdx++) {}
            if (cmdIdx >= size) {
                break;
            }
            char command = data[cmdIdx];
            switch (command) {
                case 'm': {
                    // select graphic rendition
                    if (cmdIdx == 1) {
                        // \x1b[m
                        ansiGraphicRendition(0);
                        charsToAdvance++;
                        break;
                    } else {
                        unsigned int lastSep = 0;
                        for (int i = 1; i <= cmdIdx; i++) {
                            if (data[i] == ';' || i == cmdIdx) {
                                // separator
                                if (i > lastSep + 1) {
                                    int code = stringToInt(data + lastSep + 1, i - lastSep - 1);
                                    ansiGraphicRendition(code);
                                }
                                lastSep = i;
                            }
                            charsToAdvance++;
                        }
                    }
                    break;
                }

                case 'A': {
                    // Cursor up
                    charsToAdvance++;
                    size_t amt = 1;
                    if (cmdIdx != 1) {
                        amt = stringToInt(data + 1, cmdIdx - 1);
                        charsToAdvance += cmdIdx - 1;
                    }
                    terminalY = amt > terminalY ? 0 : terminalY - amt;
                    break;
                }

                case 'B': {
                    // Cursor down
                    charsToAdvance++;
                    size_t amt = 1;
                    if (cmdIdx != 1) {
                        amt = stringToInt(data + 1, cmdIdx - 1);
                        charsToAdvance += cmdIdx - 1;
                    }
                    terminalY = (terminalY + amt >= VGAHeight) ? VGAHeight - 1 : terminalY + amt;
                    break;
                }

                case 'C': {
                    // Cursor forward
                    charsToAdvance++;
                    size_t amt = 1;
                    if (cmdIdx != 1) {
                        amt = stringToInt(data + 1, cmdIdx - 1);
                        charsToAdvance += cmdIdx - 1;
                    }
                    terminalX = (terminalX + amt >= VGAWidth) ? VGAWidth - 1 : terminalX + amt;
                    break;
                }

                case 'D': {
                    // Cursor backwards
                    charsToAdvance++;
                    size_t amt = 1;
                    if (cmdIdx != 1) {
                        amt = stringToInt(data + 1, cmdIdx - 1);
                        charsToAdvance += cmdIdx - 1;
                    }
                    terminalX = amt > terminalX ? 0 : terminalX - amt;
                    break;
                }

                case 'E': {
                    // Cursor next line
                    charsToAdvance++;
                    size_t amt = 1;
                    if (cmdIdx != 1) {
                        amt = stringToInt(data + 1, cmdIdx - 1);
                        charsToAdvance += cmdIdx - 1;
                    }
                    terminalY = (terminalY + amt >= VGAHeight) ? VGAHeight - 1 : terminalY + amt;
                    terminalX = 0;
                    break;
                }

                case 'F': {
                    // Cursor previous line
                    charsToAdvance++;
                    size_t amt = 1;
                    if (cmdIdx != 1) {
                        amt = stringToInt(data + 1, cmdIdx - 1);
                        charsToAdvance += cmdIdx - 1;
                    }
                    terminalY = amt > terminalY ? 0 : terminalY - amt;
                    terminalX = 0;
                    break;
                }

                case 'G': {
                    // Cursor absolute horizontal
                    charsToAdvance++;
                    size_t pos = 0;
                    if (cmdIdx != 1) {
                        pos = stringToInt(data + 1, cmdIdx - 1);
                        charsToAdvance += cmdIdx - 1;
                    }
                    if (pos >= VGAWidth) {
                        terminalX = VGAWidth - 1;
                    } else {
                        terminalX = pos;
                    }
                    break;
                }

                case 'J': {
                    // Erase display
                    charsToAdvance++;
                    size_t mode = 0;
                    if (cmdIdx != 1) {
                        mode = stringToInt(data + 1, cmdIdx - 1);
                        charsToAdvance += cmdIdx - 1;
                    }
                    switch (mode) {
                        case 0: {
                            // Cursor to end
                            size_t idx = terminalY * VGAWidth + terminalX;
                            for (; idx < VGAHeight * VGAWidth; idx++) {
                                terminalBuffer[idx] = vgaCharacter(' ', terminalColour);
                            }
                            break;
                        }
                        case 1: {
                            // Start to cursor
                            size_t end = terminalY * VGAWidth + terminalX;
                            for (int i = 0; i <= end; i++) {
                                terminalBuffer[i] = vgaCharacter(' ', terminalColour);
                            }
                            break;
                        }
                        case 2:
                        case 3: {
                            // Clear screen
                            // 3 is meant to delete scrollback
                            terminalClear();
                            break;
                        }
                        default: {
                            break;
                        }
                    }
                    break;
                }

                case 'K': {
                    // Erase line
                    charsToAdvance++;
                    size_t mode = 0;
                    if (cmdIdx != 1) {
                        mode = stringToInt(data + 1, cmdIdx - 1);
                        charsToAdvance += cmdIdx - 1;
                    }
                    switch (mode) {
                        case 0: {
                            // Cursor to end
                            size_t idx = terminalY * VGAWidth + terminalX;
                            for (; idx < (terminalY + 1) * VGAWidth; idx++) {
                                terminalBuffer[idx] = vgaCharacter(' ', terminalColour);
                            }
                            break;
                        }
                        case 1: {
                            // Start to cursor
                            size_t end = terminalY * VGAWidth + terminalX;
                            for (int i = terminalY * VGAWidth; i <= end; i++) {
                                terminalBuffer[i] = vgaCharacter(' ', terminalColour);
                            }
                            break;
                        }
                        case 2: {
                            // Clear entire line
                            size_t end = (terminalY + 1) * VGAWidth;
                            for (int i = terminalY * VGAWidth; i <= end; i++) {
                                terminalBuffer[i] = vgaCharacter(' ', terminalColour);
                            }
                            break;
                        }
                        default: {
                            break;
                        }
                    }
                    break;
                }

                default: {
                    charsToAdvance += cmdIdx;
                    break;
                }
            }
        } // case '['

        default: {
            break;
        }
    }
    return charsToAdvance;
}

void terminalWrite(const char* data, size_t size) {
    for (size_t i = 0; i < size; i++) {
        if (data[i] == '\x1B') {
            // ANSI escape
            if (i != size - 1) {
                i++;
                i += processANSIEscape(data + i, size - i);
            }
        } else {
            terminalPutChar(data[i]);
        }
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
