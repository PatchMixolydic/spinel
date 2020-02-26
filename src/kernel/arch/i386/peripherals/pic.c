#include <stdint.h>
#include "../core/cpu.h"
#include "pic.h"

// The PICs are here named master and subservient for no good reason

// Ports
static const uint16_t PICMasterCommandPort = 0x0020;
static const uint16_t PICMasterDataPort = 0x0021;
static const uint16_t PICSubservCommandPort = 0x00A0;
static const uint16_t PICSubservDataPort = 0x00A1;
// Commands
static const uint16_t PICReadIRRCommand = 0x0A; // IRQ register
static const uint16_t PICReadISRCommand = 0x0B; // In service register
static const uint16_t PICEndOfInterruptCommand = 0x20;
// Data
static const uint16_t PICDisableData = 0xFF;
// Flags
static const uint8_t ICW1ICW4NeededFlag = 0x01;
static const uint8_t ICW1CascadeModeFlag = 0x02;
static const uint8_t ICW1UnusedFlag = 0x04;
static const uint8_t ICW1LevelActiveFlag = 0x08;
static const uint8_t ICW1InitializeFlag = 0x10;
static const uint8_t ICW4x86ModeFlag = 0x01;
static const uint8_t ICW4AutoEOIFlag = 0x02;
static const uint8_t ICW4BufferedSubservFlag = 0x08;
static const uint8_t ICW4BufferedMasterFlag = 0x0C;
static const uint8_t ICW4FullyNestedFlag = 0x10;

static const uint8_t PICMasterOffset = 0x20;
static const uint8_t PICSubservOffset = 0x28;

void initPIC(void) {
    uint8_t masterMask = inByte(PICMasterDataPort); // Get old interrupt mask
    uint8_t subservMask = inByte(PICSubservDataPort);

    // ICW1
    outByte(PICMasterCommandPort, ICW1InitializeFlag | ICW1ICW4NeededFlag);
    outByte(PICSubservCommandPort, ICW1InitializeFlag | ICW1ICW4NeededFlag);
    // ICW2
    outByte(PICMasterDataPort, PICMasterOffset);
    outByte(PICSubservDataPort, PICSubservOffset);
    // ICW3
    outByte(PICMasterDataPort, 4); // Subservient on IRQ2 (0b0100)
    outByte(PICSubservDataPort, 2); // Master on IRQ2 (0b0010)
    // ICW4
    outByte(PICMasterDataPort, ICW4x86ModeFlag);
    outByte(PICSubservDataPort, ICW4x86ModeFlag);
    // Restore masks
    outByte(PICMasterDataPort, masterMask);
    outByte(PICSubservDataPort, subservMask);
}

void picDisable(void) {
    outByte(PICSubservDataPort, PICDisableData);
    outByte(PICMasterDataPort, PICDisableData);
}

void picEndOfInterrupt(uint8_t irq) {
    if (irq < 8) {
        outByte(PICSubservCommandPort, PICEndOfInterruptCommand);
    }
    // Must always send this to the master
    outByte(PICMasterCommandPort, PICEndOfInterruptCommand);
}

uint16_t picGetIRQRegister(void) {
    outByte(PICMasterCommandPort, PICReadIRRCommand);
    outByte(PICSubservCommandPort, PICReadIRRCommand);
    return (inByte(PICSubservCommandPort) << 8) | inByte(PICMasterCommandPort);
}

uint16_t picGetInServiceRegister(void) {
    outByte(PICMasterCommandPort, PICReadISRCommand);
    outByte(PICSubservCommandPort, PICReadISRCommand);
    return (inByte(PICSubservCommandPort) << 8) | inByte(PICMasterCommandPort);
}

void picSetIRQMasked(uint8_t irq, bool masked) {
    uint16_t port;
    if (irq < 8) {
        port = PICMasterDataPort;
    } else {
        port = PICSubservDataPort;
        irq -= 8;
    }
    uint8_t mask;
    if (masked) {
        mask = inByte(port) | (1 << irq); // add this irq to mask
    } else {
        mask = inByte(port) & ~(1 << irq); // remove this irq from mask
    }
    outByte(port, mask);
}

void picSetAllMasked(bool masked) {
    uint8_t mask = masked ? 0xFF : 0;
    outByte(PICMasterDataPort, mask);
    outByte(PICSubservDataPort, mask);
}
