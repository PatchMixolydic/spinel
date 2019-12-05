#include <limits.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

static bool print(const char* data, size_t length) {
	const unsigned char* bytes = (const unsigned char*) data;
	for (size_t i = 0; i < length; i++) {
		if (putchar(bytes[i]) == EOF) {
			return false;
		}
	}
	return true;
}

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

		const char* originalFormat = format++;

		switch (*format) {
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

			default: {
				format = originalFormat;
				size_t len = strlen(format);
				if (maxRemaining < len) {
					// TODO: Set errno to EOVERFLOW.
					return -1;
				}
				if (!print(format, len)) {
					return -1;
				}
				written += len;
				format += len;
				break;
			}
		}
	}

	va_end(parameters);
	return written;
}
