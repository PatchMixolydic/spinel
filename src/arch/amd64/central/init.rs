use crate::arch::amd64::devices::serial::SerialPort;

/// Architecture-specific device initialization.
pub fn arch_init() {
    SerialPort::Port1.initialize();
}
