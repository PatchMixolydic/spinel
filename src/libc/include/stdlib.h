#ifndef _LIBC_STDLIB_H
#define _LIBC_STDLIB_H

char* itoa(int value, char* str, int base);
char* uitoa(unsigned value, char* str, int base);

void* malloc(size_t size);
void free(void* ptr);

#endif // ndef _LIBC_STDLIB_H