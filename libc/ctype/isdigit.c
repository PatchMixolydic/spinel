#include <ctype.h>

// TODO: locale
// For now, this should suffice...
int isdigit(int c) {
    return ('0' <= c && c <= '9') ? 1 : 0;
}
