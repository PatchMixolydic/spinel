#[cfg(target_arch = "x86_64")]
pub mod amd64;

cfg_if::cfg_if! {
    if #[cfg(target_arch = "x86_64")] {
        pub use amd64::central::arch_info;
        pub use amd64::central::init::arch_init;
    }
}

/// Reexports for the virtual memory subsystem
pub mod memory {
    #[derive(Debug)]
    pub enum MapError {
        PhysicalAllocationFailed,
        AlreadyMapped,
    }

    cfg_if::cfg_if! {
        if #[cfg(target_arch = "x86_64")] {
            pub use super::amd64::memory::{
                HEAP_END,
                HEAP_SIZE,
                HEAP_START,
            };

            pub use super::amd64::memory::virtual_memory::{
                map_range,
                map_virtual_address,
                map_virtual_address_unlazily
            };
        }
    }
}

pub mod multitasking {
    #[cfg(target_arch = "x86_64")]
    pub use super::amd64::multitasking::arch_tasks;
}

pub mod interrupts {
    #[cfg(target_arch = "x86_64")]
    pub use x86_64::instructions::interrupts::{
        are_enabled as are_interrupts_enabled, disable as disable_interrupts,
        enable as enable_interrupts,
    };

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
}
