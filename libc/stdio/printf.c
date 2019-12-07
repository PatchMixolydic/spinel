#include <limits.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static const unsigned int BufferSize = 80;

static bool print(const char* data, size_t length) {
	const unsigned char* bytes = (const unsigned char*) data;
	for (size_t i = 0; i < length; i++) {
		if (putchar(bytes[i]) == EOF) {
			return false;
		}
	}
	return true;
}

// nab __itoa for our own nefarious purposes
extern char* __itoa(int value, char buf[], size_t bufSizeOrZero, int base, bool isUnsigned, bool uppercase);

int printf(const char* restrict format, ...) {
	va_list parameters;
	va_start(parameters, format);

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
				// TODO: Set errno to EOVERFLOW.
				return -1;
			}
			if (!print(format, amount)) {
				return -1;
			}
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
				char c = (char) va_arg(parameters, int); // char promotes to int
				if (!maxRemaining) {
					// TODO: Set errno to EOVERFLOW.
					return -1;
				}
				if (!print(&c, sizeof(c))) {
					return -1;
				}
				written++;
				break;
			}

			// string
			case 's': {
				format++;
				const char* str = va_arg(parameters, const char*);
				size_t len = strlen(str);
				if (maxRemaining < len) {
					// TODO: Set errno to EOVERFLOW.
					return -1;
				}
				if (!print(str, len)) {
					return -1;
				}
				written += len;
				break;
			}

			// signed integer
			case 'i': // same thing for printf
			case 'd': {
				format++;
				int number = va_arg(parameters, int);
				char buf[BufferSize];
				nitoa(number, buf, BufferSize, 10);
				int trueLen = strlen(buf);
				if (maxRemaining < trueLen) {
					// TODO: Set errno to EOVERFLOW.
					return -1;
				}
				if (!print(buf, trueLen)) {
					return -1;
				}
				written += trueLen;
				break;
			}

			// unsigned integer
			case 'u': {
				format++;
				int number = va_arg(parameters, int);
				char buf[BufferSize];
				nuitoa(number, buf, BufferSize, 10);
				int trueLen = strlen(buf);
				if (maxRemaining < trueLen) {
					// TODO: Set errno to EOVERFLOW.
					return -1;
				}
				if (!print(buf, trueLen)) {
					return -1;
				}
				written += trueLen;
				break;
			}

			// octal number
			case 'o': {
				format++;
				int number = va_arg(parameters, int);
				char buf[BufferSize];
				nuitoa(number, buf, BufferSize, 8);
				int trueLen = strlen(buf);
				if (maxRemaining < trueLen) {
					// TODO: Set errno to EOVERFLOW.
					return -1;
				}
				if (!print(buf, trueLen)) {
					return -1;
				}
				written += trueLen;
				break;
			}

			// hexidecimal number
			case 'x': // lowercase letters
			case 'X': { // uppercase letters
				bool upper = *format == 'X';
				format++;
				int number = va_arg(parameters, int);
				char buf[BufferSize];
				__itoa(number, buf, BufferSize, 16, true, upper);
				int trueLen = strlen(buf);
				if (maxRemaining < trueLen) {
					// TODO: Set errno to EOVERFLOW.
					return -1;
				}
				if (!print(buf, trueLen)) {
					return -1;
				}
				written += trueLen;
				break;
			}

			// void pointer
			case 'p': {
				format++;
				uintptr_t pointer = (uintptr_t)va_arg(parameters, void*);
				char buf[BufferSize];
				nuitoa(pointer, buf, BufferSize, 16);
				int trueLen = strlen(buf);
				if (maxRemaining < trueLen) {
					// TODO: Set errno to EOVERFLOW.
					return -1;
				}
				if (!print(buf, trueLen)) {
					return -1;
				}
				written += trueLen;
				break;
			}

			// Nothing, but outputs written to the associated int pointer parameter
			// This can apparently be exploited...
			#ifndef __is_libk
				case 'n': {
					format++;
					int* res = va_arg(parameters, int*);
					*res = written;
					break;
				}
			#endif

			// unknown
			default: {
				format -= formatSize;
				formatSize++; // account for this token
				if (maxRemaining < formatSize) {
					// TODO: Set errno to EOVERFLOW.
					return -1;
				}
				if (!print(format, formatSize)) {
					return -1;
				}
				written += formatSize;
				format += formatSize;
				break;
			}
		}
	}

	va_end(parameters);
	return written;
}
