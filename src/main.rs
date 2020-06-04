// There's a great deal of things which are left for future expansion
// or for documentation.
#![allow(dead_code)]
#![no_main]
#![no_std]
mod arch;
mod central;

#[no_mangle]
pub extern fn _start() -> ! {
    println!(
        "{} {} on {} {}",
        central::version_info::OPERATING_SYSTEM_NAME,
        central::version_info::VERSION_STRING,
        central::version_info::PROCESSOR_NAME,
        central::version_info::MACHINE_NAME
    );
    println!("Hello, Rust!");
    loop {}
}
