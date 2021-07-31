use core::hint::spin_loop;
use core::panic::PanicInfo;

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
        spin_loop();
    }
}
