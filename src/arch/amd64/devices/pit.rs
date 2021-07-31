use x86_64::instructions::port::{Port, PortWriteOnly};
use x86_64::structures::idt::InterruptStackFrame;

use crate::arch::interrupts::without_interrupts;
use crate::arch::amd64::central::idt::register_irq_handler;
use crate::arch::amd64::devices::pic::end_of_irq;
use crate::devices::timer;

const PIT_CHANNEL_0: Port<u8> = Port::new(0x0040);
const PIT_COMMAND_PORT: PortWriteOnly<u8> = PortWriteOnly::new(0x0043);

const BASE_FREQUENCY: u64 = 1193182;
/// Clock divider for the PIT. This value was decided on in Spinel classic
/// to avoid overwhelming QEMU, so this should work here.
const CLOCK_DIVIDER: u64 = 500;
const EFFECTIVE_FREQUENCY: u64 = BASE_FREQUENCY / CLOCK_DIVIDER;

#[repr(u8)]
enum Channel {
    Channel0,
    Channel1,
    Channel2,
}

#[repr(u8)]
enum AccessMode {
    /// True to its name, latches the current count.
    /// You can retrieve it by reading the latched channel's register.
    LatchCount,
    LowByteOnly,
    HighByteOnly,
    LowByteHighByte
}

#[repr(u8)]
enum OperatingMode {
    /// Upon setting this mode, the PIT waits for the reload register to be written.
    /// After receiving a reload count, the PIT will count down to 0, at which point
    /// throw output high (issuing an interrupt for channel 0) until
    /// the reload register is set once more.
    InterruptOnTerminalCount,
    /// Like `InterruptOnTerminalCount`, but waits for a rising edge from the gate input.
    /// Only channel 2 can use this.
    OneShot,
    /// Essentially a frequency divider. Once the reload register is set, the PIT count
    /// is set to the reload value on the next falling clock edge. Afterwards, if the
    /// gate signal is high on the rising edge, the count will be decremented each cycle
    /// on the falling edge. Once the timer reaches one, the output signal is thrown low.
    /// On the next falling edge, the output will be thrown high again and the count will
    /// be reloaded.
    RateGenerator,
    /// Similar to `RateGenerator`, but the output is put through a flipflop. This would
    /// divide the effective output frequency by two, but the counter decrements twice
    /// each falling edge to counteract this. Additionally, this mode transforms the output
    /// from a pulse to a square wave. Reload value is apparently rounded down to the nearest
    /// even number.
    SquareWave,
    /// Like `InterruptOnTerminalCount`, but if the gate input goes low, counting stops.
    /// TODO: is this the case for `InterruptOnTerminalCount`?
    SoftwareStrobe,
    /// Like `OneShot`
    HardwareStrobe
}

#[repr(u8)]
enum CounterMode {
    Binary,
    BCD
}

/// Generates a u8 representing the provided configuration, which can be sent out to `PIT_COMMAND_PORT`
const fn pit_command(
    channel: Channel, access_mode: AccessMode, op_mode: OperatingMode, counter_mode: CounterMode
) -> u8 {
    ((channel as u8) << 6) | ((access_mode as u8) << 4) | ((op_mode as u8) << 1) | (counter_mode as u8)
}

pub fn init() {
    without_interrupts(|| {
        // SAFETY: This function uses port mapped I/O, which can do anything.
        // Fortunately, the PIT's IO ports seem fairly well known.
        unsafe {
            PIT_COMMAND_PORT.write(
                pit_command(
                    Channel::Channel0,
                    AccessMode::LowByteHighByte,
                    OperatingMode::RateGenerator,
                    CounterMode::Binary
                )
            );
            // Write reload value as a 16-bit value
            PIT_CHANNEL_0.write((CLOCK_DIVIDER & 0xFF) as u8);
            PIT_CHANNEL_0.write(((CLOCK_DIVIDER >> 8) & 0xFF) as u8);
        }
        register_irq_handler(0.into(), irq0_handler);
        timer::init(EFFECTIVE_FREQUENCY);
    });
}

extern "x86-interrupt" fn irq0_handler(_stack_frame: InterruptStackFrame) {
    timer::tick();
    end_of_irq(0.into());
}
