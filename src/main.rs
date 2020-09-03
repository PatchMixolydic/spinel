// There's a great deal of things which are left for future expansion
// or for documentation.
#![allow(dead_code)]
#![feature(alloc_error_handler)]
#![feature(asm)]
#![feature(const_fn)]
#![feature(naked_functions)]
#![feature(const_mut_refs)]
#![feature(naked_functions)]
#![feature(panic_info_message)]
#![feature(vec_remove_item)]
#![no_main]
#![no_std]

#![cfg(any(target_arch = "x86", target_arch = "x86_64"))]
#![feature(abi_x86_interrupt)]

extern crate alloc;

/// Home of architecture specific code.
/// Some interfaces to architecture specific data may be exported from here.
mod arch;
/// Things that are central to Spinel's operation
mod central;
/// Architecture-independent devices
mod devices;
mod multitasking;

// TODO: portability!
use bootloader::BootInfo;

use arch::arch_init;
use central::{kalloc, version_info};
use multitasking::scheduler;

#[no_mangle]
pub extern "C" fn _start(boot_info: &'static BootInfo) -> ! {
    arch_init(boot_info);

    println!(
        "{} {} on {} {}",
        version_info::OPERATING_SYSTEM_NAME,
        version_info::VERSION_STRING,
        version_info::PROCESSOR_NAME,
        version_info::MACHINE_NAME
    );
    println!("The system is coming up.");
    println!("Allocator...");
    kalloc::init();
    println!("Scheduler...");
    scheduler::init();
    unreachable!();
}
