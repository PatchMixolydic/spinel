#ifndef STDLIB_H
#define STDLIB_H

#include <sys/cdefs.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

__attribute__((__noreturn__))
void abort();
// Nonstandard: convert integer to string
char* itoa(int value, char buf[], int base);
// Even more nonstandard: convert integer to string, with limit on buffer size
char* nitoa(int value, char buf[], size_t bufSize, int base);
// Nonstandard: convert unsigned integer to string
char* uitoa(int value, char buf[], size_t bufSize, int base);
// Even more nonstandard: convert unsigned integer to string, with limit on buffer size
char* nuitoa(int value, char buf[], size_t bufSize, int base);

#ifdef __cplusplus
}
#endif

#endif
