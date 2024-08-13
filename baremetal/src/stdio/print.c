#include <limits.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdarg.h>
#include "../../include/stdio.h"
#include "../../include/tty.h"
#include <string.h>

#pragma GCC diagnostic ignored "-Wsign-conversion"
#pragma GCC diagnostic ignored "-Wconversion"

static bool print(const char* data, size_t length) {
	const unsigned char* bytes = (const unsigned char*) data;
	for (size_t i = 0; i < length; i++)
		if (putchar(bytes[i]) == EOF)
			return false;
	return true;
}

static char HEX_TABLE[16] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};

int printf(const char* restrict format, ...) {
	va_list parameters;
	va_start(parameters, format);
    char integer_buf[2 + 64];

	int written = 0;

	while (*format != '\0') {
		size_t maxrem = INT_MAX - written;

		if (format[0] != '%' || format[1] == '%') {
			if (format[0] == '%')
				format++;
			size_t amount = 1;
			while (format[amount] && format[amount] != '%')
				amount++;
			if (maxrem < amount) {
				// TODO: Set errno to EOVERFLOW.
				return -1;
			}
			if (!print(format, amount))
				return -1;
			format += amount;
			written += amount;
			continue;
		}

		const char* format_begun_at = format++;

		if (*format == 'c') {
			format++;
			char c = (char) va_arg(parameters, int /* char promotes to int */);
			if (!maxrem) {
				// TODO: Set errno to EOVERFLOW.
				return -1;
			}
			if (!print(&c, sizeof(c)))
				return -1;
			written++;
		}
        else if (*format == 's') {
			format++;
			const char* str = va_arg(parameters, const char*);
			size_t len = strlen(str);
			if (maxrem < len) {
				// TODO: Set errno to EOVERFLOW.
				return -1;
			}
			if (!print(str, len))
				return -1;
			written += len;
		}
        // TODO: Generalize this to other integer sizes
        else if (*format == 'x') {
			format++;
            unsigned char num = va_arg(parameters, int);
			if (maxrem < 4) {
				// TODO: Set errno to EOVERFLOW.
				return -1;
			}
            integer_buf[0] = '0';
            integer_buf[1] = 'x';
            char high = HEX_TABLE[num / 16];
            char low = HEX_TABLE[num % 16];
            // FIXME 
            integer_buf[0] = high;
            integer_buf[1] = low;
			if (!print(integer_buf, 2))
				return -1;
			written += 2;
		}
        else if (*format == 'l') {
			format++;
            if (*format != 'x') {
                return -1;
            }
            format ++;
            unsigned long num = va_arg(parameters, int);
			if (maxrem < 10) {
				// TODO: Set errno to EOVERFLOW.
				return -1;
			}
            integer_buf[0] = '0';
            integer_buf[1] = 'x';
            for (int i = 0; i < 8; i++) {
                char ch = HEX_TABLE[(num >> ((7 - i) * 4)) % 16];
                integer_buf[i + 2] = ch;
            }
			if (!print(integer_buf, 10))
				return -1;
			written += 10;
		}

        else {
			format = format_begun_at;
			size_t len = strlen(format);
			if (maxrem < len) {
				// TODO: Set errno to EOVERFLOW.
				return -1;
			}
			if (!print(format, len))
				return -1;
			written += len;
			format += len;
		}
	}

	va_end(parameters);
	return written;
}
