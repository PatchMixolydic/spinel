#ifndef _HAKUREI_H
#define _HAKUREI_H

#include <stdbool.h>
#include <stddef.h>

/**
 * Hilarious Acronym Kept Under Respect for a shrinE maIden
 * A library providing terminal emulation support
 * Provides ANSI escape processing, colour support, varied sizes, etc.
 */

#define HakuBufferSize 64

typedef enum HakuError {
    HakuNoError,
    HakuNULLPointer,
    HakuBadSize,
    HakuNumErrors
} HakuError;

typedef enum HakuANSIColour {
    HakuANSIBlack, // Bright = dark grey
    HakuANSIRed, // Bright = light red
    HakuANSIGreen, // Bright = light green
    HakuANSIBrown, // Bright = yellow
    HakuANSIBlue, // Bright = light blue
    HakuANSIMagenta, // Bright = light magenta
    HakuANSICyan, // Bright = light cyan
    HakuANSILightGrey, // Bright = white
    HakuNumColours
} HakuANSIColour;

// Writes a character at state->x, state->y in the current colour
typedef void(*HakuPutCharCallback)(char);
// Move text cursor to state->x, state->y
typedef void(*HakuUpdateCursorCallback)(void);
// Terminal needs to be scrolled upwards
typedef void(*HakuBelowScreenCallback)(void);
typedef void(*HakuClearScreenCallback)(void);

// Please set up the callbacks!
typedef struct HakuTerminalState {
    size_t width, height;
    size_t x, y;
    bool fgBright, bgBright;
    HakuANSIColour fgColour;
    HakuANSIColour bgColour;
    HakuError lastError;

    HakuPutCharCallback putCharCallback;
    HakuUpdateCursorCallback updateCursorCallback;
    HakuBelowScreenCallback belowScreenCallback;
    HakuClearScreenCallback clearScreenCallback;
} HakuTerminalState;

HakuError hakuVerifyStateOK(HakuTerminalState* state);
void hakuWriteError(HakuTerminalState* state, char buf[HakuBufferSize]);

HakuError hakuPutString(HakuTerminalState* state, const char* str);
HakuError hakuPutStringLen(
    HakuTerminalState* state, const char* str, size_t len
);
HakuError hakuPutChar(HakuTerminalState* state, char c);

HakuError hakuMoveCursor(HakuTerminalState* state, size_t x, size_t y);
HakuError hakuClearScreen(HakuTerminalState* state);

#endif // ndef _HAKUREI_H
