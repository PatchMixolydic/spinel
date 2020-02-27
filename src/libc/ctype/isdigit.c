int isdigit(int c) {
    // TODO: locale, this might break for locales that don't use var. width 1-9
    // This is kinda balderdash but is fast. Check if the distance of this
    // character from ASCII 0 is between 0 and +9 (i.e. '0' through '9')
    return 0 <= c - '0' && c - '0' < 10;
}
