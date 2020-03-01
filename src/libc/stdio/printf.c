#include <limits.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <spinel/tty.h>

#define BufferSize 128

static const char Digits[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
static const unsigned int MinBase = 2;
static const unsigned int MaxBase = 36;

static size_t kstrlen(const char* str) {
    size_t res = 0;
    for (int i = 0; str[i] != '\0'; i++) {
        res++;
    }
    return res;
}

/* Handles base2-base36 gracefully
 * TODO: probably move this
 */
static char* _itoa(int value, char buf[], size_t bufSize, unsigned int base, bool isUnsigned) {
    size_t i = 0;
    unsigned int valueAsUnsigned = value;

    if ((base > MaxBase || base < MinBase)) {
        // no
        buf[0] = '\0';
        return buf;
    }

    if (value == 0) {
        // algorithm can't detect 0 -- do it ourselves
        buf[i++] = '0';
        buf[i] = '\0';
        return buf;
    }

    // can only signed if the base is 10
    bool isNegative = !isUnsigned && base == 10 && value < 0;
    if (isNegative) {
        value = -value;
    }

    if (!isUnsigned) {
        while (value != 0 && i < bufSize) {
            int bottomDigit = value % base;
            buf[i++] = Digits[bottomDigit];
            value /= base;
        }
    } else {
        while (valueAsUnsigned != 0 && i < bufSize) {
            int bottomDigit = valueAsUnsigned % base;
            buf[i++] = Digits[bottomDigit];
            valueAsUnsigned /= base;
        }
    }

    if (isNegative) {
        buf[i++] = '-';
    }

    buf[i] = '\0';

    char temp;
    // the string is currently reversed; fix that
    for (int start = 0, end = i - 1; start < end; start++, end--) {
        temp = buf[start];
        buf[start] = buf[end];
        buf[end] = temp;
    }

    return buf;
}

int vprintf(const char* format, va_list parameters) {
	int written = 0;
	while (*format != '\0') {
		size_t maxRemaining = INT_MAX - written;

		if (format[0] != '%' || format[1] == '%') {
			if (format[0] == '%') {
				format++;
			}
			size_t amount = 1;
			while (format[amount] && format[amount] != '%') {
				amount++;
			}
			if (maxRemaining < amount) {
				return -1;
			}
			putStringLen(format, amount);
			format += amount;
			written += amount;
			continue;
		}

		format++;
		unsigned int formatSize = 1;

		switch (*format) {
			// char
			case 'c': {
				format++;
				char c = (char)va_arg(parameters, int); // char promotes to int
				if (!maxRemaining) {
					return -1;
				}
			    putChar(c);
                updateCursor();
				written++;
				break;
			}

			// string
			case 's': {
				format++;
				const char* str = va_arg(parameters, const char*);
				size_t len = kstrlen(str);
				if (maxRemaining < len) {
					return -1;
				}
				putString(str);
				written += len;
				break;
			}

			// signed integer
			case 'i': // same thing for printf
			case 'd': {
				format++;
				int number = va_arg(parameters, int);
				char buf[BufferSize];
				_itoa(number, buf, BufferSize, 10, false);
				size_t trueLen = kstrlen(buf);
				if (maxRemaining < trueLen) {
					return -1;
				}
				putString(buf);
				written += trueLen;
				break;
			}

			// unsigned integer
			case 'u': {
				format++;
				int number = va_arg(parameters, int);
				char buf[BufferSize];
				_itoa(number, buf, BufferSize, 10, true);
				size_t trueLen = kstrlen(buf);
				if (maxRemaining < trueLen) {
					return -1;
				}
				putString(buf);
				written += trueLen;
				break;
			}

			// octal number
			case 'o': {
				format++;
				int number = va_arg(parameters, int);
				char buf[BufferSize];
				_itoa(number, buf, BufferSize, 8, true);
				size_t trueLen = kstrlen(buf);
				if (maxRemaining < trueLen) {
					return -1;
				}
				putString(buf);
				written += trueLen;
				break;
			}

			// hexidecimal number
			case 'X':
			case 'x': {
				format++;
				int number = va_arg(parameters, int);
				char buf[BufferSize];
				_itoa(number, buf, BufferSize, 16, true);
				size_t trueLen = kstrlen(buf);
				if (maxRemaining < trueLen) {
					return -1;
				}
				putString(buf);
				written += trueLen;
				break;
			}

			// void pointer
			case 'p': {
				format++;
				uintptr_t pointer = (uintptr_t)va_arg(parameters, void*);
				char buf[BufferSize];
				_itoa(pointer, buf, BufferSize, 16, true);
				size_t trueLen = kstrlen(buf);
				if (maxRemaining < trueLen) {
					return -1;
				}
				putString(buf);
				written += trueLen;
				break;
			}

			// unknown
			default: {
				format -= formatSize;
				formatSize++; // account for this token
				if (maxRemaining < formatSize) {
					return -1;
				}
				putStringLen(format, formatSize);
				written += formatSize;
				format += formatSize;
				break;
			}
		}
	} // while (*format != '\0')

	return written;
}

int printf(const char* format, ...) {
	va_list parameters;
	va_start(parameters, format);
	int written = vprintf(format, parameters);
	va_end(parameters);
    return written;
}
