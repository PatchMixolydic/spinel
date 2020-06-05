use x86_64::instructions::interrupts;

use crate::arch::amd64::central::{gdt_tss, idt};
use crate::arch::amd64::devices::{pic, serial};

/// Architecture-specific device initialization.
pub fn arch_init() {
    serial::SerialPort::Port1.initialize();
    gdt_tss::init();
    idt::init();
    pic::init();
    interrupts::enable();
}
