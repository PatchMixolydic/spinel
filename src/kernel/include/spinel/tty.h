#ifndef SPINEL_TTY_H
#define SPINEL_TTY_H

#include <stddef.h>

void putChar(char c);
void putString(const char s[]);
void putStringLen(const char s[], size_t length);
void enableCursor(void);
void disableCursor(void);
void clearScreen(void);

#endif // ndef SPINEL_TTY_H
