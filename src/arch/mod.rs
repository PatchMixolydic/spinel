#[cfg(target_arch = "x86_64")]
pub mod amd64;

cfg_if::cfg_if! {
    if #[cfg(target_arch = "x86_64")] {
        pub use amd64::central::arch_info;
        pub use amd64::central::init::arch_init;
        pub use x86_64::instructions::interrupts::{
            enable as enable_interrupts, disable as disable_interrupts
        };
    }
}
