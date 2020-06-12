// There's a great deal of things which are left for future expansion
// or for documentation.
#![allow(dead_code)]
#![feature(alloc_error_handler)]
#![feature(const_fn)]
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

use alloc::boxed::Box;
// TODO: portability!
use bootloader::BootInfo;
use core::sync::atomic::spin_loop_hint;

use arch::arch_init;
use central::{kalloc, version_info};
use devices::timer;

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
    kalloc::init();
    timer::register_timer(timer::TICKS_PER_SECOND / 2, false, Box::new(|| {
        print!(".")
    })).unwrap();
    timer::register_timer(timer::TICKS_PER_SECOND * 4, true, Box::new(|| {
        print!("!")
    })).unwrap();

    loop {
        spin_loop_hint();
    }
}
