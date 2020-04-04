#include <stdio.h>
#include <string.h>

char* strtok_r(char* str, const char* delimiters, char** state) {
    if (str == NULL && (state == NULL || *state == NULL)) {
        return NULL;
    }

    char* searchStr = str == NULL ? *state : str;

    // Find the first non-delimiter character
    char* res = searchStr + strspn(searchStr, delimiters);
    if (res > searchStr + strlen(searchStr)) {
        // there are no tokens in here
        *state = NULL;
        return NULL;
    }

    // set the next delimiter to NUL, if there are any
    char* nextDelimiter = strpbrk(res, delimiters);
    if (nextDelimiter != NULL) {
        // found next delimiter
        *nextDelimiter = '\0';
        *state = ++nextDelimiter;
    } else {
        // no more delimiters, no more tokens
        *state = NULL;
    }

    // Update state
    return res;
}
