#ifndef PS2KEYBOARD_H
#define PS2KEYBOARD_H

// Early initialization of PS/2 devices
// This is for devices connected over USB legacy,
// so comprehensive initialization will be skipped
// (apparently it can cause issues?)
// Let's hope the PS/2 devices work at this stage
void initPS2USBLegacy(void);
// Okay, we've scuttled USB legacy emulation. This is for actual PS/2 devices
// This will do comprehensive initialization and testing
void initPS2(void);

#endif // ndef PS2KEYBOARD_H
