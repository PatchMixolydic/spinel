//! Memory map
//! * 0x0000_0000_0000_0000 - 0x0000_7FFF_FFFF_FFFF - userspace
//! * 0x0000_8000_0000_0000 - 0xFFFF_7FFF_FFFF_FFFF - inaccessible to 48-bit amd64 processors
//! * 0xFFFF_8000_0000_0000 - 0xFFFF_8000_0000_FFFF - kernel stack
//! * 0xFFFF_8000_0001_0000 - 0xFFFF_8000_FFFF_FFFF - kernel heap
//! * 0xFFFF_8001_0000_0000 - 0xFFFF_8001_0000_0FFF - bootloader information
//! * 0xFFFF_8001_0000_1000 - 0xFFFF_8FFF_FFFF_FFFF - reserved
//! * 0xFFFF_9000_0000_0000 - 0xFFFF_CFFF_FFFF_FFFF - physical memory
//! * 0xFFFF_D000_0000_0000 - 0xFFFF_FFFF_FFFF_FFFF - reserved
// TODO: is this the right place for this?

pub mod physical_memory;
pub mod virtual_memory;

pub const PHYSICAL_MEMORY_OFFSET: u64 = 0xFFFF_9000_0000_0000;
pub const HEAP_START: usize = 0xFFFF_8000_0001_0000;
// 0x0001_0000_0000 - 0x0001_0000 ≈ 4 GiB
pub const HEAP_SIZE: usize = 0xFFFF_0000;
pub const HEAP_END: usize = HEAP_START + HEAP_SIZE;
