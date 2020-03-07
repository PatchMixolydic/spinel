#include <ctype.h>
#include <limits.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#ifdef __LIBCKERN
#include <spinel/tty.h>
#endif

// Chosen to fit enough octal numerals to fit the maximum 64-bit unsigned int
#define BufferSize 22
// TODO: can this be made into a static function? I don't know how the va_list
// would take to being modified
#define PrintNumber(val, base, convFunc) do {\
	char buf[BufferSize];\
	convFunc(val, buf, base);\
	size_t trueLen = strlen(buf);\
	if (maxRemaining < trueLen) {\
		return -1;\
	}\
	__print(buf, trueLen);\
	written += trueLen;\
} while (0)

// TODO: stdout
static void __print(const char* str, size_t len) {
	#ifdef __LIBCKERN
		putStringLen(str, len);
	#else
		(void)str;
		(void)len;
		#warning "__print not implemented for hosted"
	#endif
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

		// In most cases, we will want to advance past the format character
		// We will go ahead and just skip ahead here
		format += 2;
		unsigned int formatSize = 1;

		switch (*(format - 1)) {
			// char
			case 'c': {
				char c = (char)va_arg(parameters, int); // char promotes to int
				if (!maxRemaining) {
					return -1;
				}
			    __print(&c, 1);
				written++;
				break;
			}

			// string
			case 's': {
				const char* str = va_arg(parameters, const char*);
				size_t len = strlen(str);
				if (maxRemaining < len) {
					return -1;
				}
				__print(str, len);
				written += len;
				break;
			}

			// signed integer
			case 'i': // same thing for printf
			case 'd': {
				int number = va_arg(parameters, int);
				PrintNumber(number, 10, itoa);
				break;
			}

			// unsigned integer
			case 'u': {
				int number = va_arg(parameters, int);
				PrintNumber(number, 10, uitoa);
				break;
			}

			// octal number
			case 'o': {
				int number = va_arg(parameters, int);
				PrintNumber(number, 8, itoa);
				break;
			}

			// hexidecimal number
			case 'X':
			case 'x': {
				int number = va_arg(parameters, int);
				char buf[BufferSize];

				itoa(number, buf, 16);
				size_t trueLen = strlen(buf);
				if (maxRemaining < trueLen) {
					return -1;
				}

				// Correct for lowercase
				if (*(format - 1) == 'x') {
					for (size_t i = 0; i < trueLen; i++) {
						if (isalpha(buf[i])) {
							buf[i] += 'a' - 'A';
						}
					}
				}

				__print(buf, trueLen);
				written += trueLen;
				break;
			}

			// void pointer
			case 'p': {
				uintptr_t pointer = (uintptr_t)va_arg(parameters, void*);
				PrintNumber(pointer, 16, itoa);
				break;
			}

			// unknown
			default: {
				format -= formatSize + 1;
				formatSize++; // account for this token
				if (maxRemaining < formatSize) {
					return -1;
				}
				__print(format, formatSize);
				written += formatSize;
				format += formatSize;
				break;
			}
		}
	} // while (*format != '\0')

	return written;
}
