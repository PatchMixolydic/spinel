use core::panic::PanicInfo;
use core::sync::atomic::spin_loop_hint;

use crate::{print, println};

#[panic_handler]
fn panic(info: &PanicInfo) -> ! {
    print!("panic: ");

    if info.message().is_some() {
        println!("{}", info.message().unwrap());
        if info.location().is_some() {
            println!("Panicked at {}", info.location().unwrap());
        }
    } else {
        println!("{:?}", info)
    }

    loop {
        spin_loop_hint();
    }
}
