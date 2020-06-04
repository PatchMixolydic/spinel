#[cfg(target_arch = "x86_64")]
pub mod amd64;

#[cfg(target_arch = "x86_64")]
pub use amd64::central::arch_info;
#[cfg(target_arch = "x86_64")]
pub use amd64::central::init::arch_init;
