// Note: this is sorta arch specific, but it's used by x86{,_64}
// and PowerPC Macs, so there's not really a good home for this otherwise.

#ifdef SpinelWantPCI

#include <stdint.h>
#include <stdio.h>
#include <sys/endian.h>
#include <spinel/pci.h>

#ifdef __i386__
    #include "../arch/i386/core/cpu.h"
#elif defined __powerpc__
    #error "Please add PowerPC support for PCI"
#else
    #error "Current architecture doesn't support PCI yet"
#endif

// PCI isn't exactly a device, but it's a device bus, so here it is.

// TODO: ACPI allows for memory-mapped PCI tables, but ACPI is not supported yet

// See pci.h
#define PCIOffset(name, id) const uint8_t PCI##name##Offset = id
EnumeratePCIOffsets
#undef PCIOffset


static const uint32_t AddressEnable = 0x80000000;

static const uint16_t PCIAddressPort = 0xCF8;
static const uint16_t PCIDataPort = 0xCFC;

const uint8_t PCIGeneralType = 0x00;
const uint8_t PCIPCIBridgeType = 0x01;
const uint8_t PCICardBusType = 0x02;
const uint8_t PCIMultifuncTypeBit = 0x80;

static inline uint32_t getAddress(
    uint8_t bus, uint8_t dev, uint8_t func, uint8_t offset
) {
    // align offset to uint32_t
    return htole32(
        AddressEnable | ((uint32_t)bus << 16) | ((uint32_t)dev << 11) |
        ((uint32_t)func << 8) | ((uint32_t)offset & 0xFC)
    );
}

static void enumerateBus(uint8_t bus) {
    for (uint8_t dev = 0; dev < 32; dev++) {
        for (uint8_t func = 0; func < 8; func++) {
            if (pciReadConfig16(bus, dev, func, PCIVendorIDOffset) == 0xFFFF) {
                // Invalid
                break;
            }

            uint8_t headerType = pciReadConfig8(
                bus, dev, func, PCIHeaderTypeOffset
            );

            if ((headerType & ~PCIMultifuncTypeBit) == PCIPCIBridgeType) {
                enumerateBus(pciReadConfig8(
                    bus, dev, func, PCISecondBusOffset
                ));
            }

            // TODO: drivers
            printf(
                "Found device: %X:%X.%X %X:%X\n", bus, dev, func,
                pciReadConfig16(bus, dev, func, PCIVendorIDOffset),
                pciReadConfig16(bus, dev, func, PCIDeviceIDOffset)
            );

            // Check if this is multifunction
            if (!(headerType & PCIMultifuncTypeBit)) {
                break;
            }
        }
    }
}

void pciEnumerateDevices(void) {
    // TODO: apparently the multifunction bit can be checked to see if there's
    // multiple buses; is this necessary?

    printf("Enumerating PCI devices...\n");
    for (uint8_t bus = 0; bus < 8; bus++) {
        if (pciReadConfig16(0, 0, bus, PCIVendorIDOffset) == 0xFFFF) {
            // Oops, this doesn't exist
            continue;
        }
        enumerateBus(bus);
    }
    printf("PCI device enumeration complete\n");
}

uint32_t pciReadConfig32(
    uint8_t bus, uint8_t dev, uint8_t func, uint8_t offset
) {
    uint32_t address = getAddress(bus, dev, func, offset);
    // TODO: this isn't portable
    outDWord(PCIAddressPort, address);
    return le32toh(inDWord(PCIDataPort));
}

uint16_t pciReadConfig16(
    uint8_t bus, uint8_t dev, uint8_t func, uint8_t offset
) {
    uint32_t cfgWord = pciReadConfig32(bus, dev, func, offset);
    // Get the word-selecting bit
    uint32_t byteOffset = (offset & 2) ? 0 : 16;
    // Endianness was already handled by pciReadConfig32
    return (uint16_t)((cfgWord >> byteOffset) & 0xFFFF);
}

uint8_t pciReadConfig8(
    uint8_t bus, uint8_t dev, uint8_t func, uint8_t offset
) {
    uint32_t cfgWord = pciReadConfig32(bus, dev, func, offset);
    // offset & 3 determines the byte
    // 8 * (3 - byteDeterminant) is the offset
    // 0 -> 24, 1 -> 16, 2 -> 8, 3 -> 0
    uint32_t byteOffset = 8 * (3 - (offset & 3));
    return (uint8_t)((cfgWord >> byteOffset) & 0xFF);
}

#endif // def SpinelWantPCI
