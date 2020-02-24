#ifndef SPINEL_TTY_H
#define SPINEL_TTY_H

void putChar(char c);
void putString(char s[]);
void enableCursor();
void disableCursor();
void updateCursor();
void clearScreen();

#endif // ndef SPINEL_TTY_H
