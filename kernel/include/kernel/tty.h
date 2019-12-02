#ifndef _KERNEL_TTY_H
#define _KERNEL_TTY_H

#include <stdint.h>
#include <stddef.h>

void terminalInitialize();
void terminalClear();
void terminalSetColour(uint8_t color);
void terminalPutChar(char c);
void terminalWrite(const char* data, size_t size);
void terminalWriteString(const char* data);
void terminalEnableCursor(uint8_t startLine, uint8_t endLine);
void terminalDisableCursor();
void terminalMoveCursor(size_t x, size_t y);

#endif
