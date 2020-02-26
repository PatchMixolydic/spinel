#include <spinel/ansi.h>

// TODO: This might go well in a library -- libspinel or something? libspinelansi?

int parseANSIEscape(const char s[]) {
    if(s[0] != '\x1B') {
        // That's not an ANSI escape
        return 0;
    }
    return 0;
}
