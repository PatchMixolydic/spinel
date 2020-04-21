#include <stdbool.h>
#include <stdint.h>
#include <spinel/clock.h>
#include <spinel/cpu.h>
#include "cmos.h"
#include "../core/cpu.h"

static const uint16_t RegisterSelectPort = 0x70;
static const uint16_t CMOSDataPort = 0x71;

static const uint8_t SecondsRegister = 0x00;
static const uint8_t MinutesRegister = 0x02;
static const uint8_t HoursRegister = 0x04;
static const uint8_t DayRegister = 0x07;
static const uint8_t MonthRegister = 0x08;
// 0-99
static const uint8_t YearRegister = 0x09;
static const uint8_t StatusRegisterA = 0x0A;
static const uint8_t StatusRegisterB = 0x0B;
// TODO: This isn't guaranteed
// The actual value is stored in the ACPI tables,
// but ACPI is not supported yet.
static const uint8_t CenturyRegister = 0x32;

// Set on each register selection
static const uint8_t NMIDisableBit = 0x80;
// Found in Status Register A
static const uint8_t RTCUpdateInProgressBit = 0x80;
// Found in Status Register B
// Determines if values are in binary or BCD
static const uint8_t RTCBinaryBit = 0x04;
// The hours register has two modes: 12 hour and 24 hour
// In 12 hour mode, the MSbit is set post meridiem.
static const uint8_t HourPMBit = 0x80;

ClockTime* cmosGetTime(void);

static ClockSource rtcSource = {
    cmosGetTime
};

static ClockTime currentTime = {
    .year = 0,
    .month = 0,
    .day = 0,
    .hour = 0,
    .minute = 0,
    .second = 0
};

static bool nmiEnabled = true;
// Sometimes the RTC uses BCD instead of binary numbers
static bool rtcUsesBCD = false;

static inline uint8_t setNMIBitForRegister(uint8_t reg) {
    return nmiEnabled ? reg & ~NMIDisableBit : reg | NMIDisableBit;
}

static inline uint8_t readRegister(uint8_t reg) {
    // Select register
    bool oldNMIStatus = nmiEnabled;
    // Prevent any interrupts from overwriting our selected register
    disableInterrupts();
    setNMIEnabled(false);
    outByte(RegisterSelectPort, setNMIBitForRegister(reg));
    uint8_t res = inByte(CMOSDataPort);
    setNMIEnabled(oldNMIStatus);
    enableInterrupts();
    return res;
}

static inline uint8_t readRegisterRTC(uint8_t reg) {
    uint8_t res = readRegister(reg);
    if (rtcUsesBCD) {
        // Convert to decimal
        // Get top hex digit, multiply by 10, add bottom hex digit
        // res / 16 * 10 + (res & 0x0F)
        // but let's optimize it :P
        res = (((res & 0xF0) >> 3) + ((res & 0xF0) >> 1)) + (res & 0x0F);
    }
    return res;
}

static inline void writeRegister(uint8_t reg, uint8_t val) {
    bool oldNMIStatus = nmiEnabled;
    disableInterrupts();
    setNMIEnabled(false);
    outByte(RegisterSelectPort, setNMIBitForRegister(reg));
    outByte(CMOSDataPort, val);
    enableInterrupts();
    setNMIEnabled(oldNMIStatus);
}

static inline void waitForRTCUpdateDone(void) {
    while (readRegister(StatusRegisterA) & RTCUpdateInProgressBit) {
        // TODO: block
        __asm__("pause");
    }
}

void initCMOS(void) {
    rtcUsesBCD = !(readRegister(StatusRegisterB) & RTCBinaryBit);
    registerClockSource(&rtcSource);
}

ClockTime* cmosGetTime(void) {
    ClockTime take1;
    ClockTime take2;
    uint8_t take1Century;
    uint8_t take2Century;
    while (true) {
        // Read the clock once...
        waitForRTCUpdateDone();
        take1Century = readRegisterRTC(CenturyRegister);
        take1.year = readRegisterRTC(YearRegister);
        take1.month = readRegisterRTC(MonthRegister);
        take1.day = readRegisterRTC(DayRegister);
        take1.hour = readRegisterRTC(HoursRegister);
        take1.minute = readRegisterRTC(MinutesRegister);
        take1.second = readRegisterRTC(SecondsRegister);

        // An update might have started after
        // checking if an update was in progress,
        // so let's double check our values...
        waitForRTCUpdateDone();
        take2Century = readRegisterRTC(CenturyRegister);
        take2.year = readRegisterRTC(YearRegister);
        take2.month = readRegisterRTC(MonthRegister);
        take2.day = readRegisterRTC(DayRegister);
        take2.hour = readRegisterRTC(HoursRegister);
        take2.minute = readRegisterRTC(MinutesRegister);
        take2.second = readRegisterRTC(SecondsRegister);

        if (clockTimesEqual(&take1, &take2) && take1Century == take2Century) {
            break;
        }
        // Otherwise, keep going until we get consistency
    }

    currentTime.year = take2Century * 100 + take2.year;
    currentTime.month = take2.month;
    currentTime.day = take2.day;

    if (take2.hour & HourPMBit) {
        // 12-hour mode and it's PM
        currentTime.hour = take2.hour & ~HourPMBit;
        // 12p = 12:00, 1p = 13:00, 2p = 14:00...
        currentTime.hour += currentTime.hour == 12 ? 0 : 12;
    } else {
        if (take2.hour == 12) {
            // 12 hour mode and it's 12am
            currentTime.hour = 0;
        } else {
            // For 24 or 12 hour mode at 1-11am, this is ok
            currentTime.hour = take2.hour;
        }
    }

    currentTime.minute = take2.minute;
    currentTime.second = take2.second;

    return &currentTime;
}

void setNMIEnabled(bool state) {
    if (state == nmiEnabled) {
        return;
    }

    nmiEnabled = state;
    uint8_t reg = inByte(RegisterSelectPort);
    outByte(RegisterSelectPort, setNMIBitForRegister(reg));
}
