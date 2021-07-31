use core::fmt;
use core::hint::spin_loop;
use x86_64::instructions::port::{Port, PortReadOnly, PortWriteOnly};

#[derive(Clone, Copy)]
pub enum SerialPort {
    Port1,
    Port2,
    Port3,
    Port4,
}

impl From<SerialPort> for u16 {
    fn from(port: SerialPort) -> u16 {
        match port {
            SerialPort::Port1 => 0x03F8,
            SerialPort::Port2 => 0x02F8,
            SerialPort::Port3 => 0x03E8,
            SerialPort::Port4 => 0x02E8,
        }
    }
}

impl SerialPort {
    /// Initialize a serial port.
    /// Currently assumes 8N1 @ 115200 baud.
    pub fn initialize(self) {
        // Clock is 115200 Hz
        self.write_divisor(1);

        // SAFETY: This reads from and writes to port I/O,
        // which can do anything. Fortunately, the
        // serial ports seem to be well-defined
        // constants.
        unsafe {
            self.interrupt_enable_register().write(0); // No interrupts
            self.line_control_register().write(0b0000_0011); // 8N1
                                                             // Trigger level 14 bytes(?), 16B FIFO, DMA off, clear Tx/Rx, enable
            self.fifo_control_register().write(0b1100_0111);
            // Enable request to send, data terminal ready lines
            self.modem_control_register().write(0b0000_0011);
        }
    }

    pub fn read(self) -> u8 {
        unsafe {
            // Spin while no data available (check data ready bit)
            while self.line_status_register().read() & 0b0000_0001 == 0 {
                spin_loop();
            }
            self.data_register().read()
        }
    }

    pub fn write(self, val: u8) {
        unsafe {
            // Spin while line is busy (check empty Tx bit)
            while self.line_status_register().read() & 0b0010_0000 == 0 {
                spin_loop();
            }
            self.data_register().write(val)
        }
    }

    /// Writes to the divisor register.
    /// Note that this function enables the divisor latch access bit
    /// on its own and, more importantly, disables it at the end
    /// of the function.
    fn write_divisor(self, divisor: u16) {
        // SAFETY: This reads from and writes to port I/O,
        // which can do anything.
        unsafe {
            // Enable divisor latch access bit
            self.line_control_register()
                .write(self.line_control_register().read() | 0x80);

            self.divisor_register().write(divisor);

            // Clear DLAB
            self.line_control_register()
                .write(self.line_control_register().read() & !0x80);
        }
    }

    fn data_register(self) -> Port<u8> {
        Port::new(u16::from(self))
    }

    fn divisor_register(self) -> Port<u16> {
        Port::new(u16::from(self))
    }

    fn interrupt_enable_register(self) -> Port<u8> {
        Port::new(u16::from(self) + 1)
    }

    fn interrupt_id_register(self) -> PortReadOnly<u8> {
        PortReadOnly::new(u16::from(self) + 2)
    }

    fn fifo_control_register(self) -> PortWriteOnly<u8> {
        PortWriteOnly::new(u16::from(self) + 2)
    }

    fn line_control_register(self) -> Port<u8> {
        Port::new(u16::from(self) + 3)
    }

    fn modem_control_register(self) -> Port<u8> {
        Port::new(u16::from(self) + 4)
    }

    fn line_status_register(self) -> PortReadOnly<u8> {
        PortReadOnly::new(u16::from(self) + 5)
    }
}

impl fmt::Write for SerialPort {
    fn write_str(&mut self, s: &str) -> Result<(), fmt::Error> {
        for character in s.bytes() {
            self.write(character);
        }
        Ok(())
    }
}
