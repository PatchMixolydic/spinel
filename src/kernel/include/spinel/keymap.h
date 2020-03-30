#ifndef SPINEL_KEYMAP_H
#define SPINEL_KEYMAP_H

#include <stdbool.h>
#include <stdint.h>

void setKeyCode(uint8_t keyCode, bool status);
bool getKeyCode(uint8_t keyCode);

#endif // ndef SPINEL_KEYMAP_H
