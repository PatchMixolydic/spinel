#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <spinel/keymap.h>
#include <spinel/tty.h>
#include "../core/cpu.h"
#include "../interrupts/interrupts.h"
#include "pic.h"
#include "ps2.h"
#include "ps2Keyboard.h"

// Interrupt-based keyboard driver for PS/2 keyboards

// Commands for keyboards
static const uint8_t EchoCommand = 0xEE;
static const uint8_t ScanCodeCommand = 0xF0;
static const uint8_t IdentifyCommand = 0xF2;
static const uint8_t EnableScanCommand = 0xF4;
static const uint8_t DisableScanCommand = 0xF5;
static const uint8_t ResendCommand = 0xFE;
static const uint8_t RestartCommand = 0xFF;

// We may get some responses from the keyboard
static const uint8_t Error1Resp = 0;
static const uint8_t TestPassedResp = 0xAA;
static const uint8_t EchoResp = 0xEE;
static const uint8_t AckResp = 0xFA;
// Sent after RestartCommand if self-testing fails
static const uint8_t TestFailed1Resp = 0xFC;
static const uint8_t TestFailed2Resp = 0xFD;
static const uint8_t ResendResp = 0xFE;
static const uint8_t Error2Resp = 0xFF;

static const unsigned MaxSendAttempts = 3;

// TODO: might require different keymaps for different regions?
// There has to be a better way...
// Comments show mapping to US QWERTY
static uint8_t scanCodeToKeyCode[256] = {
//  none  F9    none  F5    F3    F1    F2    F12   none  F10   F8    F6
    0xFF, 0x09, 0xFF, 0x05, 0x03, 0x01, 0x02, 0x0C, 0xFF, 0x0A, 0x08, 0x06,
//  F4    Tab   `     none  none  lAlt  lShft none  lCtrl Q     1     none
    0x04, 0x40, 0x20, 0xFF, 0xFF, 0xA2, 0x80, 0xFF, 0xA0, 0x41, 0x21, 0xFF,
//  none  none  Z     S     A     W     2     none  none  C     X     D
    0xFF, 0xFF, 0x81, 0x62, 0x61, 0x42, 0x22, 0xFF, 0xFF, 0x83, 0x82, 0x63,
//  E     4     3     none  none  spc   V     F     T     R     5     none
    0x43, 0x24, 0x23, 0xFF, 0xFF, 0xA3, 0x84, 0x64, 0x45, 0x44, 0x25, 0xFF,
//  none  N     B     H     G     Y     6     none  none  none  M     J
    0xff, 0x86, 0x85, 0x66, 0x65, 0x46, 0x26, 0xFF, 0xFF, 0xFF, 0x87, 0x67,
//  U     7     8     none  none  ,     K     I     O     0     9     none
    0x47, 0x27, 0x28, 0xFF, 0xFF, 0x88, 0x68, 0x48, 0x49, 0x2A, 0x29, 0xFF,
//  none  .     /     L     ;     P     -     none  none  none  '     none
    0xFF, 0x89, 0x8A, 0x69, 0x6A, 0x4A, 0x2B, 0xFF, 0xFF, 0xFF, 0x6B, 0xFF,
//  [     =     none  none  Caps  rShft Enter ]     none  \     none  none
    0x4B, 0x2C, 0xFF, 0xFF, 0x60, 0x8B, 0x6C, 0x4C, 0xFF, 0x4D, 0xFF, 0xFF,
//  none  none  none  none  none  none  Bksp  none  none  Kp1   none  Kp4
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x2D, 0xFF, 0xFF, 0x8D, 0xFF, 0x6D,
//  Kp7   none  none  none  Kp0   Kp.   Kp2   Kp5   Kp6   Kp8   Esc   NumLk
    0x51, 0xFF, 0xFF, 0xFF, 0xAB, 0xAC, 0x8E, 0x6E, 0x6F, 0x52, 0x00, 0x31,
//  F11   Kp+   Kp3   Kp-   Kp*   Kp9   ScrLk none  none  none  none  F7
    0x0B, 0x54, 0x8F, 0x34, 0x33, 0x53, 0x0E, 0xFF, 0xFF, 0xFF, 0xFF, 0x07,
    // all scan codes after this map to none
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF
};

// scan codes introduced with 0xE0
static uint8_t extendedScanCodeToKeyCode[256];

static uint8_t recvData(void) {
    // wait for the keyboard's response
    while (!(inByte(PS2StatusPort) & OutputBufferFullFlag)) {
        // TODO: block
        __asm__("pause");
    }
    return inByte(PS2DataPort);
}

static bool sendData(uint8_t data, unsigned port) {
    while ((volatile uint8_t)inByte(PS2StatusPort) & InputBufferFullFlag) {
        // TODO: block
        __asm__("pause");
    }

    for (unsigned attempts = MaxSendAttempts; attempts > 0; attempts--) {
        switch (port) {
            case 1:
                break;
            case 2:
                outByte(PS2CommandPort, WritePS2Port2InputCommand);
                break;
            default:
                printf("Tried to send 0x%X to PS/2 port %u\n", data, port);
                return false;
        }
        outByte(PS2DataPort, data);

        uint8_t res = recvData();
        if (res == ResendResp) {
            // try again
            continue;
        } else if (res == AckResp) {
            // okay!
            return true;
        } else {
            // odd
            return false;
        }
    }
    // Failed
    return false;
}

void initPS2USBLegacy(void) {
    // Early boot, we're using USB Legacy emulation because the USB drivers
    // aren't loaded yet. Still, let's try to set scancode set 2.
    // First, disable translation
    outByte(PS2CommandPort, GetConfigByteCommand);
    uint8_t config = recvData();
    config &= ~PS2Port1TransFlag;
    outByte(PS2CommandPort, SetConfigByteCommand);
    while ((volatile uint8_t)inByte(PS2StatusPort) & InputBufferFullFlag) {
        __asm__("pause");
    }
    outByte(PS2DataPort, config);

    if (!sendData(ScanCodeCommand, 1)) {
        // uh oh
        return;
    }
}

void initPS2(void) {
    // Real PS/2 initialization
    // TODO
}

void irq1(void) {
    // The PS/2 keyboard sent us this because it has data ready.
    uint8_t scancode = inByte(PS2DataPort);
    if (scancode == AckResp) {
        // This is an acknowledgement, not a keycode!
        picEndOfInterrupt(1);
        return;
    }

    uint8_t* keyCodeMap = scanCodeToKeyCode;
    bool isDown = true;

    if (scancode == 0xE0) {
        // extended
        keyCodeMap = extendedScanCodeToKeyCode;
        scancode = recvData();
    }

    if (scancode == 0xF0) {
        // Up
        isDown = false;
        scancode = recvData();
    }

    if (scancode == 0xE1) {
        // Hello, pause...
        // Eat the rest of the scancode
        recvData();
        recvData();
        recvData();
        recvData();
        recvData();
    } else {
        setKeyCode(keyCodeMap[scancode], isDown);
    }

    picEndOfInterrupt(1);
}
