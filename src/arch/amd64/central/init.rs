use bootloader::BootInfo;
use x86_64::instructions::interrupts;

use crate::arch::amd64::central::{gdt_tss, idt};
use crate::arch::amd64::devices::{pic, pit, serial};
use crate::arch::amd64::memory::{physical_memory, virtual_memory};

/// Architecture-specific device initialization.
/// This function has a couple of important responsibilities:
/// * Bring up I/O, if needed
/// * Bring up the memory subsystem
pub fn arch_init(boot_info: &'static BootInfo) {
    serial::SerialPort::Port1.initialize();
    gdt_tss::init();
    idt::init();
    pic::init();
    interrupts::enable();
    // SAFETY: virtual_memory::init requires that physical memory is identity mapped
    // at `boot_info.physical_memory_offset`. physical_memory::init requires that the
    // memory_map is set up correctly. Fortunately, the bootloader guarantees this!
    unsafe {
        physical_memory::init(&boot_info.memory_regions);
        virtual_memory::init();
    }
    pit::init();
    pic::set_irq_masked(0.into(), false);
}
