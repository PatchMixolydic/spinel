#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include "hakurei.h"

// These messages must be at most HakuBufferSize long
static const char ErrorStrings[][HakuBufferSize] = {
    "No error recorded\n", // HakuNoError
    "NULL in argument or callback\n", // HakuNULLPointer
    "Height and/or width is zero\n", // HakuBadSize
};

static void newLine(HakuTerminalState* state) {
    // assumes state is ok
    state->x = 0;
    state->y++;
    if (state->y >= state->height) {
        state->belowScreenCallback();
        state->y = state->height - 1;
    }
}

HakuError hakuVerifyStateOK(HakuTerminalState* state) {
    if (
        state == NULL || state->putCharCallback == NULL ||
        state->updateCursorCallback == NULL ||
        state->belowScreenCallback == NULL ||
        state->clearScreenCallback == NULL
    ) {
        state->lastError = HakuNULLPointer;
        return HakuNULLPointer;
    }

    if (state->height == 0 || state->width == 0) {
        state->lastError = HakuBadSize;
        return HakuBadSize;
    }
    return HakuNoError;
}

void hakuWriteError(HakuTerminalState* state, char buf[HakuBufferSize]) {
    if (state != NULL && state->lastError < HakuNumErrors) {
        strlcpy(buf, ErrorStrings[state->lastError], HakuBufferSize);
    } else if (state == NULL) {
        strlcpy(buf, "Gave NULL to hakuWriteError\n", HakuBufferSize);
    } else {
        strlcpy(buf, "Unknown error\n", HakuBufferSize);
    }
}

HakuError hakuPutString(HakuTerminalState* state, const char str[]) {
    return hakuPutStringLen(state, str, strlen(str));
}

HakuError hakuPutStringLen(
    HakuTerminalState* state, const char str[], size_t len
) {
    HakuError err = hakuVerifyStateOK(state);
    if (err != HakuNoError) {
        return err;
    }

    if (str == NULL) {
        return HakuNULLPointer;
    }

    for (size_t i = 0; i < len && str[i] != '\0'; i++) {
        if (str[i] == '\x1B') {
            // ANSI escape
            // TODO: continue parsing ANSI escapes across calls
            i += hakuParseANSI(state, str + i) - 1;
        } else {
            hakuPutChar(state, str[i]);
        }
    }

    state->updateCursorCallback();

    return HakuNoError;
}

HakuError hakuPutChar(HakuTerminalState* state, char c) {
    HakuError err = hakuVerifyStateOK(state);
    if (err != HakuNoError) {
        return err;
    }

    switch (c) {
        case '\n': {
            newLine(state);
            break;
        }

        case '\r': {
            state->x = 0;
            break;
        }

        case '\b': {
            if (state->x > 0) {
                state->x--;
            } else {
                state->x = state->width - 1;
                state->y--;
            }
            state->putCharCallback(' ');
            break;
        }

        case '\x1B': {
            // stray escape... ignore
            // TODO: start ANSI parser instead
            break;
        }

        default: {
            state->putCharCallback(c);
            state->x++;
            break;
        }
    }

    if (state->x >= state->width) {
        newLine(state);
    }

    return HakuNoError;
}

HakuError hakuMoveCursor(HakuTerminalState* state, size_t x, size_t y) {
    HakuError err = hakuVerifyStateOK(state);
    if (err != HakuNoError) {
        return err;
    }

    if (x >= state->width) {
        state->x = state->width;
    } else {
        state->x = x;
    }

    state->y = y;
    state->updateCursorCallback();

    return HakuNoError;
}

HakuError hakuClearScreen(HakuTerminalState* state) {
    HakuError err = hakuVerifyStateOK(state);
    if (err != HakuNoError) {
        return err;
    }

    state->clearScreenCallback();
    state->x = 0;
    state->y = 0;
    state->updateCursorCallback();

    return HakuNoError;
}
