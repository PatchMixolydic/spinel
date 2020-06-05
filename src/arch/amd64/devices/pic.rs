//! Programmable Interrupt Controller

use x86_64::instructions::port::Port;

const COMMAND_END_OF_INTERRUPT: u8 = 0x20;
const COMMAND_READ_IN_SERVICE_REGISTER: u8 = 0x0B;

const MASTER_INTERRUPT_OFFSET: u8 = 0x20;
const SUBSERVIENT_INTERRUPT_OFFSET: u8 = 0x28;

const NUM_IRQS: u8 = 16;

/// Wrapper for a u8 that enforces that its contents
/// represent a valid IRQ.
///
/// It implements From<u8>, so for pretty much any
/// function that takes an IRQ, you can use `into()`:
/// ```
/// fn foo(irq: IRQ) {
///     // ...
/// }
///
/// foo(1.into());
/// ```
#[repr(transparent)]
pub struct IRQ {
    id: u8
}

impl From<u8> for IRQ {
    fn from(val: u8) -> Self {
        assert!(val < 16);
        IRQ {id: val}
    }
}

/// Initialize the PIC and remap interrupts. Does not enable interrupts.
pub fn init() {
    // SAFETY: This reads from and writes to port I/O, which might do anything
    unsafe {
        // Initialize, will provide control word 4
        master_command_port().write(0b0001_0001);
        subservient_command_port().write(0b0001_0001);

        // CW2: set interrupt offsets
        master_data_port().write(MASTER_INTERRUPT_OFFSET);
        subservient_data_port().write(SUBSERVIENT_INTERRUPT_OFFSET);

        // CW3: PIC configuration
        master_data_port().write(0b0000_0100); // Subserv. at IRQ2
        subservient_data_port().write(0b0000_0010); // "Cascade identity", says OSDev Wiki

        // CW4: This is an x86(-like)
        master_data_port().write(0b0000_0001);
        subservient_data_port().write(0b0000_0001);

        // Mask all
        set_all_irqs_masked(true);
    }
}

/// Sends the end of interrupt signal to the PIC
pub fn end_of_interrupt(irq: IRQ) {
    // SAFETY: This reads from and writes to port I/O, which might do anything
    unsafe {
        if irq.id >= 8 {
            // Subservient PIC generated this one
            subservient_command_port().write(COMMAND_END_OF_INTERRUPT);
        }
        // Master is always informed
        master_command_port().write(COMMAND_END_OF_INTERRUPT);
    }
}

// We're initializing port and relative_irq simultaneously,
// but Clippy wants us to assign the if expression to relative_irq
#[allow(clippy::useless_let_if_seq)]
pub fn set_irq_masked(irq: IRQ, masked: bool) {
    let mut port;
    // IRQ to this PIC
    let relative_irq;

    if irq.id < 8 {
        port = master_data_port();
        relative_irq = irq.id;
    } else {
        port = subservient_data_port();
        relative_irq = irq.id - 8;
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
    let value = if masked {0xFF} else {0x00};
    // SAFETY: This reads from and writes to port I/O, which might do anything
    unsafe {
        master_data_port().write(value);
        subservient_data_port().write(value);
    }
}

/// Reads the in service register to check if an interrupt has actually occured
pub fn read_isr() -> u16 {
    // SAFETY: This reads from and writes to port I/O, which might do anything
    unsafe {
        master_command_port().write(COMMAND_READ_IN_SERVICE_REGISTER);
        subservient_command_port().write(COMMAND_READ_IN_SERVICE_REGISTER);
        (u16::from(master_command_port().read()) << 8) | u16::from(subservient_command_port().read())
    }
}

fn master_command_port() -> Port<u8> {
    Port::new(0x0020)
}

fn master_data_port() -> Port<u8> {
    Port::new(0x0021)
}

fn subservient_command_port() -> Port<u8> {
    Port::new(0x00A0)
}

fn subservient_data_port() -> Port<u8> {
    Port::new(0x00A1)
}
