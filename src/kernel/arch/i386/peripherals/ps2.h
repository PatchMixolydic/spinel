#ifndef PS2_H
#define PS2_H

#include <stdint.h>

static const uint16_t PS2DataPort = 0x60;
static const uint16_t PS2StatusPort = 0x64;
static const uint16_t PS2CommandPort = 0x64;

// status flags
// "Output" and "input" output from and input to the device
// Input is the computer's output, output is the computer's input
static const uint8_t OutputBufferFullFlag = 1;
static const uint8_t InputBufferFullFlag = 2;
static const uint8_t POSTPassedFlag = 4;
static const uint8_t InputIsCommandFlag = 8;
static const uint8_t ChipsetSpecific1Flag = 16;
static const uint8_t ChipsetSpecific2Flag = 32;
static const uint8_t TimeoutFlag = 64;
static const uint8_t ParityErrorFlag = 128;

// Configuration byte flags
static const uint8_t PS2Port1InterruptFlag = 1;
static const uint8_t PS2Port2InterruptFlag = 2;
// static const uint8_t POSTPassedFlag = 4;
static const uint8_t Unused1Flag = 8;
// These two flags are logically inverted, 1 = false 0 = true
static const uint8_t PS2Port1ClockNotFlag = 16;
static const uint8_t PS2Port2ClockNotFlag = 32;
static const uint8_t PS2Port1TransFlag = 64;
static const uint8_t Unused2Flag = 128;

// Controller output port flags
// 0 activates the reset line, this should be pulsed to perform a reset
static const uint8_t CPUResetNotFlag = 1;
static const uint8_t A20GateFlag = 2;
static const uint8_t PS2Port2ClockFlag = 4;
static const uint8_t PS2Port2DataFlag = 8;
static const uint8_t OutputFromPort1Flag = 16;
static const uint8_t OutputFromPort2Flag = 32;
static const uint8_t PS2Port1ClockFlag = 64;
static const uint8_t PS2Port1DataFlag = 128;

// Controller commands
static const uint8_t GetConfigByteCommand = 0x20;
static const uint8_t SetConfigByteCommand = 0x60;
static const uint8_t DisablePS2Port2Command = 0xA7;
static const uint8_t EnablePS2Port2Command = 0xA8;
static const uint8_t TestPS2Port2Command = 0xA9;
static const uint8_t TestPS2ControllerCommand = 0xAA;
static const uint8_t TestPS2Port1Command = 0xAB;
static const uint8_t DisablePS2Port1Command = 0xAD;
static const uint8_t EnablePS2Port1Command = 0xAE;
static const uint8_t ReadControllerOutputCommand = 0xD0;
// These two commands write the next byte on the data port as requested
static const uint8_t WriteControllerOutputCommand = 0xD1;
static const uint8_t WritePS2Port2InputCommand = 0xD4;
// Resets the CPU (the x86, not the PS/2 controller)
static const uint8_t PulseCPUResetCommand = 0xF0;

#endif // ndef PS2_H
