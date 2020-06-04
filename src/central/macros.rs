#[macro_export]
macro_rules! print {
    ($($arg:tt)*) => ($crate::central::macros::_print(format_args!($($arg)*)));
}

#[macro_export]
macro_rules! println {
    () => ($crate::print!("\n"));
    ($($arg:tt)*) => ($crate::print!("{}\n", format_args!($($arg)*)));
}

#[doc(hidden)]
pub fn _print(args: core::fmt::Arguments) {
    use core::fmt::Write;
    use crate::arch::amd64::devices::text_mode_tty::WRITER;
    WRITER.lock().write_fmt(args).unwrap();

    if cfg!(all(target_arch = "x86_64", feature = "serial_logging")) {
        use crate::arch::amd64::devices::serial::SerialPort;
        SerialPort::Port1.write_fmt(args).unwrap();
    }
}

