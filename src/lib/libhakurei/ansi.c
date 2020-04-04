#include "hakurei.h"

#define GetIntParameter(param) do {\
    if (str < commandLoc) {\
        param = __getIntParameter(&str);\
    }\
} while (0)

// Returns the integer read, modifies str to point past integer
static unsigned __getIntParameter(const char** str) {
    unsigned res = 0;
    // Look while the current byte is a digit
    for(; **str != '\0' && '0' <= **str && **str <= '9'; (*str)++) {
        res *= 10;
        res += (unsigned)(**str - '0');
    }
    if (0x30 <= **str && **str <= 0x3F) {
        // this is another parameter byte, probably a seperator
        (*str)++;
    }
    return res;
}

int hakuParseANSI(HakuTerminalState* state, const char str[]) {
    HakuError err = hakuVerifyStateOK(state);
    if (err != HakuNoError) {
        return 0;
    }

    if (str == NULL || str[0] != '\x1B' || 0x40 > str[1] || str[1] > 0x5F) {
        // not a valid escape sequence
        return 0;
    }

    int escapeLen = 2; // skip escape character and type at least
    str++; // go to sequence type

    switch (str[0]) {
        case '[': {
            // Control sequence introducer
            // Seek forward to find the command
            char command = '\0';
            char* commandLoc = NULL;
            for (int i = 1; str[i] != '\0'; i++) {
                if (0x40 <= str[i] && str[i] <= 0x7E) {
                    // found command byte
                    command = str[i];
                    commandLoc = (char*)str + i;
                    escapeLen += i;
                    break;
                }
            }

            // Go to the first parameter byte (if any)
            str++;

            switch (command) {
                case 'A': {
                    // cursor up
                    unsigned numLines = 1;
                    GetIntParameter(numLines);
                    size_t newY = state->y - numLines;
                    if (newY > state->y) {
                        // underflow
                        newY = 0;
                    }
                    hakuMoveCursor(state, state->x, newY);
                    break;
                }

                case 'B': {
                    // cursor down
                    unsigned numLines = 1;
                    GetIntParameter(numLines);
                    size_t newY = state->y + numLines;
                    if (newY >= state->height) {
                        newY = state->height - 1;
                    }
                    hakuMoveCursor(state, state->x, newY);
                    break;
                }

                case 'C': {
                    // cursor forward
                    unsigned numLines = 1;
                    GetIntParameter(numLines);
                    size_t newX = state->x + numLines;
                    if (newX >= state->width) {
                        newX = state->width - 1;
                    }
                    hakuMoveCursor(state, newX, state->y);
                    break;
                }

                case 'D': {
                    // cursor backward
                    unsigned numLines = 1;
                    GetIntParameter(numLines);
                    size_t newX = state->x - numLines;
                    if (newX > state->x) {
                        newX = 0;
                    }
                    hakuMoveCursor(state, newX, state->y);
                    break;
                }

                case 'E': {
                    // cursor next line
                    unsigned numLines = 1;
                    GetIntParameter(numLines);
                    size_t newY = state->y + numLines;
                    if (newY >= state->height) {
                        newY = state->height - 1;
                    }
                    hakuMoveCursor(state, 0, newY);
                    break;
                }

                case 'F': {
                    // cursor previous line
                    unsigned numLines = 1;
                    GetIntParameter(numLines);
                    size_t newY = state->y - numLines;
                    if (newY > state->y) {
                        newY = 0;
                    }
                    hakuMoveCursor(state, 0, newY);
                    break;
                }

                case 'G': {
                    // cursor horizontal absolute
                    unsigned newX = 1;
                    GetIntParameter(newX);
                    newX--; // values are one based in ANSI but zero based here
                    if (newX >= state->width) {
                        newX = state->width - 1;
                    }
                    hakuMoveCursor(state, newX, state->y);
                    break;
                }

                case 'f':
                case 'H': {
                    // cursor position
                    unsigned newX = 1;
                    unsigned newY = 1;
                    GetIntParameter(newX);
                    GetIntParameter(newY);
                    newX--;
                    newY--;
                    if (newX >= state->width) {
                        newX = state->width - 1;
                    }
                    if (newY >= state->height) {
                        newY = state->height - 1;
                    }
                    hakuMoveCursor(state, newX, newY);
                    break;
                }

                case 'J': {
                    // erase in display
                    unsigned mode = 0;
                    GetIntParameter(mode);
                    // mode 0 - clear from cursor to end
                    // mode 1 - clear from beginning to cursor
                    // mode 2 - clear screen
                    // mode 3 - clear screen and delete scrollback
                    // TODO
                    if (mode == 2 || mode == 3) {
                        hakuClearScreen(state);
                    }
                    break;
                }

                case 'm': {
                    // set graphics rendition
                    unsigned param = 0;

                    do {
                        GetIntParameter(param);
                        if (param == 0 || param == 22) {
                            // reset / normal colour
                            state->fgColour = state->defaultFGColour;
                            state->bgColour = state->defaultBGColour;
                            state->fgBright = state->defaultFGBright;
                            state->bgBright = state->defaultBGBright;
                        } else if (param == 1) {
                            // bold / intense
                            state->fgBright = true;
                        } else if (param == 2) {
                            // faint
                            state->fgBright = false;
                        } else if (30 <= param && param <= 37) {
                            // set foreground colour
                            state->fgColour = (HakuANSIColour)(param - 30);
                        } else if (param == 39) {
                            // default foreground colour
                            state->fgColour = state->defaultFGColour;
                            state->fgColour = state->defaultFGBright;
                        } else if (40 <= param && param <= 47) {
                            // set background colour
                            state->bgColour = (HakuANSIColour)(param - 40);
                        } else if (param == 49) {
                            // default background colour
                            state->bgColour = state->defaultBGColour;
                            state->bgColour = state->defaultBGBright;
                        } else if (90 <= param && param <= 97) {
                            // set bright foreground colour
                            state->fgColour = (HakuANSIColour)(param - 90);
                            state->fgBright = true;
                        } else if (100 <= param && param <= 107) {
                            // set bright background colour
                            state->bgColour = (HakuANSIColour)(param - 100);
                            state->bgBright = true;
                        }
                    } while (str < commandLoc);

                    break;
                }

                default: {
                    // none
                    break;
                }
            }

            break;
        }

        case 'c': {
            // Reset to initial state
            state->fgColour = state->defaultFGColour;
            state->bgColour = state->defaultBGColour;
            break;
        }

        default: {
            // unknown
            break;
        }
    }

    return escapeLen;
}
