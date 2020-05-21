#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <spinel/concurrency.h>
#include <spinel/arch/i386/cpu.h>
#include <spinel/arch/i386/pic.h>
#include <spinel/arch/i386/serial.h>

const unsigned SerialMaxBaudRate = 115200;

// Each serial port has several ports at various offsets
// When DLAB is high, 0 and 1 become the divisor for the baud rate
// For Tx/Rx, just use the port
static const uint16_t InterruptOffset = 1;
static const uint16_t DivLowOffset = 0;
static const uint16_t DivHighOffset = 1;
// Read: identify interrupt, write: FIFO control
static const uint16_t IntrIDFIFOCtlOffset = 2;
// MSB of line ctrl port is the divisor latch access bit (DLAB)
static const uint16_t LineCtrlOffset = 3;
static const uint16_t ModemCtrlOffset = 4;
static const uint16_t LineStatusOffset = 5;
static const uint16_t ModemStatusOffset = 6;
static const uint16_t ScratchOffset = 7;

static Mutex serialPortMutexes[4] = {0, 0, 0, 0};

static void initSerialPort(uint16_t port, uint16_t baud) {
    // Disable interrupts
    outByte(port + InterruptOffset, 0);
    setSerialBaudRate(port, baud);
    // LCR
    // bit 7 - DLAB
    // bits 5 - 3 - parity
    // bit 2 - number of stop bits
    // bits 1-0 - number of data bits
    // 8 bit, no parity, 1 stop bit = 0b0000_0011 = 0x03
    outByte(port + LineCtrlOffset, 0x03);
    // Enable FIFO, clear buffers, set trigger level of FIFOs
    // Bits 7-6 - FIFO trigger level
    // Bit 3 - DMA
    // Bit 2 - Clear Tx
    // Bit 1 - Clear Rx
    // Bit 0 - Enable
    // 0b1100_0111
    outByte(port + IntrIDFIFOCtlOffset, 0xC7);
    // Enable interrupts, RTS, DTR
    // Bit 4 - Loopback
    // Bit 3 - UART interrupts go to interrupt control unit
    // Bit 2 - Typically unused
    // Bit 1 - Enable -RTS
    // Bit 0 - Enable -DTR
    outByte(port + ModemCtrlOffset, 0x0B);
    // Interrupts
    // Bit 3 - Modem status interrupt
    // Bit 2 - Status interrupt
    // Bit 1 - Tx buffer empty interrupt
    // Bit 0 - Data ready interrupt
    // It seems like since most serial I/O will be through a file most likely,
    // we don't need/shouldn't use interrupts (we can just spin~)
}

// Convert from an I/O port ID to its number
// ex. serialPortToNum(SerialPort1) == 1
static inline unsigned serialPortToNum(uint16_t serialPort) {
    switch (serialPort) {
        case SerialPort1:
            return 1;

        case SerialPort2:
            return 2;

        case SerialPort3:
            return 3;

        case SerialPort4:
            return 4;

        default:
            return 0;
    }
}

void initSerial(unsigned baudRate) {
    initSerialPort(SerialPort1, baudRate);
    initSerialPort(SerialPort2, baudRate);
    initSerialPort(SerialPort3, baudRate);
    initSerialPort(SerialPort4, baudRate);
    // Enable IRQs
    picSetIRQMasked(3, false);
    picSetIRQMasked(4, false);
}

void setSerialBaudRate(uint16_t serialPort, unsigned baudRate) {
    if (baudRate > SerialMaxBaudRate) {
        baudRate = SerialMaxBaudRate;
    }
    // The serial controllers use a divisor to determine baud rate
    uint16_t divisor = SerialMaxBaudRate / baudRate;
    // Set the DLAB
    outByte(
        serialPort + LineCtrlOffset, inByte(serialPort + LineCtrlOffset) | 0x80
    );
    // Send the divisor
    outByte(serialPort + DivLowOffset, (uint8_t)(divisor & 0xFF));
    outByte(serialPort + DivHighOffset, (uint8_t)((divisor >> 8) & 0xFF));
    // Clear the DLAB
    outByte(
        serialPort + LineCtrlOffset, inByte(serialPort + LineCtrlOffset) & 0x7F
    );
}

uint8_t serialRead(uint16_t serialPort) {
    // Poll until line is clear
    while ((inByte(serialPort + LineStatusOffset) & 1) == 0) {
        __asm__("pause");
    }
    return inByte(serialPort);
}

void serialWrite(uint16_t serialPort, uint8_t val) {
    // Poll until Tx buffer is clear
    while ((inByte(serialPort + LineStatusOffset) & 0x20) == 0) {
        __asm__("pause");
    }
    outByte(serialPort, val);
}

void serialWriteStrLen(uint16_t serialPort, const char str[], size_t len) {
    spinlockMutex(&serialPortMutexes[serialPortToNum(serialPort)]);
    for (size_t i = 0; i < len && str[i] != '\0'; i++) {
        serialWrite(serialPort, str[i]);
    }
    unlockMutex(&serialPortMutexes[serialPortToNum(serialPort)]);
}

void serialWriteStr(uint16_t serialPort, const char str[]) {
    serialWriteStrLen(serialPort, str, strlen(str));
}
