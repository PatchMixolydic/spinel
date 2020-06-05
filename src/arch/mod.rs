#[cfg(target_arch = "x86_64")]
pub mod amd64;

cfg_if::cfg_if! {
    if #[cfg(target_arch = "x86_64")] {
        pub use amd64::central::arch_info;
        pub use amd64::central::init::arch_init;
        pub use x86_64::instructions::interrupts::{
            are_enabled as are_interrupts_enabled,
            enable as enable_interrupts,
            disable as disable_interrupts
        };
    }
}

/// Run a function with interrupts disabled.
/// Afterwards, this function restores interrupts to their
/// previous state, whether they were enabled or disabled.
pub fn without_interrupts<T>(f: impl FnOnce() -> T) {
    let should_enable = are_interrupts_enabled();
    disable_interrupts();
    f();
    if should_enable {
        enable_interrupts();
    }
}
