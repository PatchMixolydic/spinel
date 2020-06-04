pub use crate::arch::arch_info::{MACHINE_NAME, PROCESSOR_NAME};

pub const OPERATING_SYSTEM_NAME: &'static str = "Spinel";

macro_rules! version_number {
    ($major: literal, $minor: literal, $patche: literal) => {
        /// Is an i32 too ambitious? Not ambitious enough? Who knows~
        pub const VERSION_MAJOR: i32 = $major;
        pub const VERSION_MINOR: i32 = $minor;
        pub const VERSION_PATCH: i32 = $patche;
        /// String representation of the major, minor, and patch versions
        pub const VERSION_STRING: &'static str = concat!($major, '.', $minor, '.', $patche);
    };
}

version_number!(0, 1, 0);
