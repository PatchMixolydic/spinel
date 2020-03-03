#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include "hakurei.h"

// These messages must be at most HakuBufferSize long
static const char* const * const ErrorStrings = {
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

HakuError hakuPutString(HakuTerminalState* state, const char* str) {
    return hakuPutStringLen(state, str, strlen(str));
}

HakuError hakuPutStringLen(
    HakuTerminalState* state, const char* str, size_t len
) {
    HakuError err = hakuVerifyStateOK(state);
    if (err != HakuNoError) {
        return err;
    }

    for (size_t i = 0; i < len && str[i] != '\0'; i++) {
        switch (str[i]) {
            case '\n': {
                newLine(state);
                break;
            }

            case '\r': {
                state->x = 0;
                break;
            }

            default: {
                hakuPutChar(state, str[i]);
                break;
            }
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

    state->putCharCallback(c);
    state->x++;

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

    state->x = x;
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
