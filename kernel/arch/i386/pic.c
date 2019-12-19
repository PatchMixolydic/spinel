#include <stdint.h>
#include "pic.h"
#include "io.h"

// The PICs are here named master and subservient for no good reason

// Ports
const uint16_t PICMasterCommandPort = 0x0020;
const uint16_t PICMasterDataPort = 0x0021;
const uint16_t PICSubservientCommandPort = 0x00A0;
const uint16_t PICSubservientDataPort = 0x00A1;
// Commands
const uint16_t PICReadIRRCommand = 0x0A; // IRQ register
const uint16_t PICReadISRCommand = 0x0B; // In service register
const uint16_t PICEndOfInterruptCommand = 0x20;
// Data
const uint16_t PICDisableData = 0xFF;
// Flags
const uint8_t ICW1ICW4NeededFlag = 0x01;
const uint8_t ICW1CascadeModeFlag = 0x02;
const uint8_t ICW1UnusedFlag = 0x04;
const uint8_t ICW1LevelActiveFlag = 0x08;
const uint8_t ICW1InitializeFlag = 0x10;
const uint8_t ICW4x86ModeFlag = 0x01;
const uint8_t ICW4AutoEOIFlag = 0x02;
const uint8_t ICW4BufferedSubservFlag = 0x08;
const uint8_t ICW4BufferedMasterFlag = 0x0C;
const uint8_t ICW4FullyNestedFlag = 0x10;

/**
 * Initialize the PICs
 * @param masterOffset The offset between IRQ and interrupt for the master PIC
 * @param subservOffset The offset between IRQ and interrupt for the subservient PIC
 */
void picInitialize(int masterOffset, int subservOffset) {
    uint8_t masterMask = inByte(PICMasterDataPort); // Get old interrupt mask
    uint8_t subservMask = inByte(PICSubservientDataPort);

    // ICW1
    outByte(PICMasterCommandPort, ICW1InitializeFlag | ICW1ICW4NeededFlag);
    outByte(PICSubservientCommandPort, ICW1InitializeFlag | ICW1ICW4NeededFlag);
    // ICW2
    outByte(PICMasterDataPort, masterOffset);
    outByte(PICSubservientDataPort, subservOffset);
    // ICW3
    outByte(PICMasterDataPort, 4); // Subservient on IRQ2 (0b0100)
    outByte(PICSubservientDataPort, 2); // Master on IRQ2 (0b0010)
    // ICW4
    outByte(PICMasterDataPort, ICW4x86ModeFlag);
    outByte(PICSubservientDataPort, ICW4x86ModeFlag);
    // Restore masks
    outByte(PICMasterDataPort, masterMask);
    outByte(PICSubservientDataPort, subservMask);
}

void picDisable() {
    outByte(PICSubservientDataPort, PICDisableData);
    outByte(PICMasterDataPort, PICDisableData);
}

void picEndOfInterrupt(bool toSubserv) {
    if (toSubserv) {
        outByte(PICSubservientCommandPort, PICEndOfInterruptCommand);
    }
    // Must always send this to the master
    outByte(PICMasterCommandPort, PICEndOfInterruptCommand);
}

uint16_t picGetIRQRegister() {
    outByte(PICMasterCommandPort, PICReadIRRCommand);
    outByte(PICSubservientCommandPort, PICReadIRRCommand);
    // We are indeed reading the command port. IBM PCs sure are weird
    return (inByte(PICSubservientCommandPort) << 8) | inByte(PICMasterCommandPort);
}

uint16_t picGetInServiceRegister() {
    outByte(PICMasterCommandPort, PICReadISRCommand);
    outByte(PICSubservientCommandPort, PICReadISRCommand);
    return (inByte(PICSubservientCommandPort) << 8) | inByte(PICMasterCommandPort);
}

void picSetIRQMasked(uint8_t irq, bool masked) {
    uint16_t port;
    if (irq < 8) {
        port = PICMasterDataPort;
    } else {
        port = PICSubservientDataPort;
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
