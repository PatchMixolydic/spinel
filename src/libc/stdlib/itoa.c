#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>

static const char ItoaDigits[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";

static void reverse(char* str, size_t len) {
    char tmp;
	for (size_t start = 0, end = len - 1; start < end; start++, end--) {
        tmp = str[start];
        str[start] = str[end];
        str[end] = tmp;
    }
}

char* itoa(int value, char* str, int base) {
    if (base != 10) {
        // Only decimal numbers are handled as signed
        return uitoa((unsigned)value, str, base);
    }

	size_t i = 0;
    bool negative = value < 0;

    // Value needs to be positive or else the string will not be constructed
    // correctly
    if (negative) {
        value = -value;
    }

	while (value != 0) {
		str[i++] = ItoaDigits[value % base];
		value /= base;
	}

	// Check for negative
	if (negative) {
		str[i++] = '-';
	}

	// The string constructed is now backwards, reverse it
    reverse(str, i);
    str[i] = '\0';

	return str;
}

// Since these share ItoaDigits and reverse, I have decided to put these
// in the same file... I hope that's okay
char* uitoa(unsigned value, char* str, int base) {
	size_t i = 0;

	while (value != 0) {
		str[i++] = ItoaDigits[value % base];
		value /= base;
	}

	// The string constructed is now backwards, reverse it
    reverse(str, i);
    str[i] = '\0';

	return str;
}
