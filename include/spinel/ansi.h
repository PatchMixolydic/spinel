#ifndef SPINEL_ANSI_H
#define SPINEL_ANSI_H

// Takes a string starting with an ANSI escape and parses it.
// Returns the number of characters to skip to skip the escape code.
int parseANSIEscape(char s[]);

#endif // ndef SPINEL_ANSI_H
