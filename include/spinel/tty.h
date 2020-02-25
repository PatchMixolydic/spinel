#ifndef SPINEL_TTY_H
#define SPINEL_TTY_H

void putChar(char c);
void putString(char s[]);
void enableCursor(void);
void disableCursor(void);
void updateCursor(void);
void clearScreen(void);

#endif // ndef SPINEL_TTY_H
