#ifndef SPINEL_TTY_H
#define SPINEL_TTY_H

void putChar(char c);
void putString(char s[]);
void enableCursor();
void disableCursor();
void moveCursor(unsigned int x, unsigned int y);

#endif // ndef SPINEL_TTY_H
