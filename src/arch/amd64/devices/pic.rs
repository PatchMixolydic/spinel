use x86_64::instructions::port::Port;

use crate::arch::amd64::central::idt::IRQ;

const MASTER_COMMAND_PORT: Port<u8> = Port::new(0x0020);
const MASTER_DATA_PORT: Port<u8> = Port::new(0x0021);
const SUBSERVIENT_COMMAND_PORT: Port<u8> = Port::new(0x00A0);
const SUBSERVIENT_DATA_PORT: Port<u8> = Port::new(0x00A1);

const COMMAND_END_OF_INTERRUPT: u8 = 0x20;
const COMMAND_READ_IN_SERVICE_REGISTER: u8 = 0x0B;

const MASTER_INTERRUPT_OFFSET: u8 = 0x20;
const SUBSERVIENT_INTERRUPT_OFFSET: u8 = 0x28;

/// Initialize the PIC and remap interrupts. Does not enable interrupts.
pub fn init() {
    // SAFETY: This reads from and writes to port I/O, which might do anything
    unsafe {
        // Initialize, will provide control word 4
        MASTER_COMMAND_PORT.write(0b0001_0001);
        SUBSERVIENT_COMMAND_PORT.write(0b0001_0001);

        // CW2: set interrupt offsets
        MASTER_DATA_PORT.write(MASTER_INTERRUPT_OFFSET);
        SUBSERVIENT_DATA_PORT.write(SUBSERVIENT_INTERRUPT_OFFSET);

        // CW3: PIC configuration
        MASTER_DATA_PORT.write(0b0000_0100); // Subserv. at IRQ2
        SUBSERVIENT_DATA_PORT.write(0b0000_0010); // "Cascade identity", says OSDev Wiki

        // CW4: This is an x86(-like)
        MASTER_DATA_PORT.write(0b0000_0001);
        SUBSERVIENT_DATA_PORT.write(0b0000_0001);

        // Mask all
        set_all_irqs_masked(true);
    }
}

/// Sends the end of interrupt signal to the PIC
pub fn end_of_irq(irq: IRQ) {
    // SAFETY: This reads from and writes to port I/O, which might do anything
    unsafe {
        if irq.id() >= 8 {
            // Subservient PIC generated this one
            SUBSERVIENT_COMMAND_PORT.write(COMMAND_END_OF_INTERRUPT);
        }
        // Master is always informed
        MASTER_COMMAND_PORT.write(COMMAND_END_OF_INTERRUPT);
    }
}

// We're initializing port and relative_irq simultaneously,
// but Clippy wants us to assign the if expression to relative_irq
#[allow(clippy::useless_let_if_seq)]
pub fn set_irq_masked(irq: IRQ, masked: bool) {
    let mut port;
    // IRQ to this PIC
    let relative_irq;

    if irq.id() < 8 {
        port = MASTER_DATA_PORT;
        relative_irq = irq.id();
    } else {
        port = SUBSERVIENT_DATA_PORT;
        relative_irq = irq.id() - 8;
    }

    // SAFETY: This reads from and writes to port I/O, which might do anything
    unsafe {
        let masked_value = if masked {
            port.read() | 1 << relative_irq
        } else {
            port.read() & !(1 << relative_irq)
        };

        port.write(masked_value);
    }
}

pub fn set_all_irqs_masked(masked: bool) {
    let value = if masked { 0xFF } else { 0x00 };
    // SAFETY: This reads from and writes to port I/O, which might do anything
    unsafe {
        MASTER_DATA_PORT.write(value);
        SUBSERVIENT_DATA_PORT.write(value);
    }
}

/// Reads the in service register to check if an interrupt has actually occured
pub fn read_isr() -> u16 {
    // SAFETY: This reads from and writes to port I/O, which might do anything
    unsafe {
        MASTER_COMMAND_PORT.write(COMMAND_READ_IN_SERVICE_REGISTER);
        SUBSERVIENT_COMMAND_PORT.write(COMMAND_READ_IN_SERVICE_REGISTER);
        (u16::from(MASTER_COMMAND_PORT.read()) << 8) | u16::from(SUBSERVIENT_COMMAND_PORT.read())
    }
}
