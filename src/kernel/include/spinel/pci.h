#if defined(SpinelWantPCI) && !defined(SPINEL_PCI_H)
#define SPINEL_PCI_H

// Sorta neat, sorta hacky trick to avoid code duplication
// For each PCIOffset(name, id), creates a PCINameOffset with
// the given ID
// PCINameOffset is defined as extern here and is actually defined
// in pci.c

#define EnumeratePCIOffsets \
PCIOffset(DeviceID, 0x00);\
PCIOffset(VendorID, 0x02);\
PCIOffset(Status, 0x04);\
PCIOffset(Command, 0x06);\
PCIOffset(ClassCode, 0x08);\
PCIOffset(Subclass, 0x09);\
PCIOffset(ProgramIface, 0x0A);\
PCIOffset(Revision, 0x0B);\
PCIOffset(SelfTest, 0x0C);\
PCIOffset(HeaderType, 0x0D);\
PCIOffset(LatencyTimer, 0x0E);\
PCIOffset(CacheLineSize, 0x0F);\
/* The following variables depend on the header type */\
/* General */\
PCIOffset(BaseAddr0, 0x10);\
PCIOffset(BaseAddr1, 0x14);\
PCIOffset(BaseAddr2, 0x18);\
PCIOffset(BaseAddr3, 0x1C);\
PCIOffset(BaseAddr4, 0x20);\
PCIOffset(BaseAddr5, 0x24);\
PCIOffset(CardBusCIS, 0x28);\
PCIOffset(SubsysID, 0x2C);\
PCIOffset(SubsysVendID, 0x2E);\
PCIOffset(ExpROMGeneral, 0x30);\
PCIOffset(Capabilities, 0x37);\
PCIOffset(MaxLatency, 0x3C);\
PCIOffset(BurstPeriod, 0x3D);\
PCIOffset(InterruptPin, 0x3E);\
PCIOffset(InterruptLine, 0x3F);\
/* PCI to PCI bridge */\
/* BaseAddr{0,1}, Capabilities, Interrupt{Pin,Line} are the same */\
PCIOffset(LatencyTimer2, 0x18);\
PCIOffset(SubordBus, 0x19);\
PCIOffset(SecondBus, 0x1A);\
PCIOffset(PrimaryBus, 0x1B);\
PCIOffset(SecondaryStatus, 0x1C);\
PCIOffset(IOLimitLow, 0x1E);\
PCIOffset(IOBaseLow, 0x1F);\
PCIOffset(MemoryLimit, 0x20);\
PCIOffset(MemoryBase, 0x22);\
PCIOffset(PrefetchLimitLow, 0x24);\
PCIOffset(PrefetchBaseLow, 0x26);\
PCIOffset(PrefetchBaseTop, 0x28);\
PCIOffset(PrefetchLimitTop, 0x2C);\
PCIOffset(IOLimitTop, 0x30);\
PCIOffset(IOBaseTop, 0x32);\
PCIOffset(ExpROMPCIBridge, 0x38);\
PCIOffset(BridgeCtrl, 0x3C);\
/* TODO: CardBus support if necessary */\

#define PCIOffset(name, id) extern const uint8_t PCI##name##Offset
EnumeratePCIOffsets
#undef PCIOffset

extern const uint8_t PCIGeneralType;
extern const uint8_t PCIPCIBridgeType;
extern const uint8_t PCICardBusType;
extern const uint8_t PCIMultifuncTypeBit;

uint8_t pciReadConfig8(
    uint8_t bus, uint8_t dev, uint8_t func, uint8_t offset
);
uint16_t pciReadConfig16(
    uint8_t bus, uint8_t dev, uint8_t func, uint8_t offset
);
uint32_t pciReadConfig32(
    uint8_t bus, uint8_t dev, uint8_t func, uint8_t offset
);
void pciEnumerateDevices(void);

#endif // defined(SpinelWantPCI) && !defined(SPINEL_PCI_H)
