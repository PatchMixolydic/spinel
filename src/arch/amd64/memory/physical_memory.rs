use bootloader::bootinfo::{MemoryMap, MemoryRegionType};
use spin::Mutex;
use x86_64::PhysAddr;
use x86_64::structures::paging::{FrameAllocator, PhysFrame, Size4KiB};

use crate::arch::arch_info::PAGE_SIZE;

/// The page frame allocation manager. It is not recommended to use this directly,
/// but sometimes it is necessary for interfacing with the `x86_64` crate.
pub static PAGE_FRAME_ALLOCATOR: Mutex<Option<PageFrameAllocator>> = Mutex::new(None);

/// Behind-the-scenes memory allocator.
/// You probably shouldn't construct one of these outside of `physical_memory`.
pub struct PageFrameAllocator {
    memory_map: &'static MemoryMap,
    next: usize
}

impl PageFrameAllocator {
    /// Create a new `PageFrameAllocator`.
    ///
    /// ## Safety
    /// The `MemoryMap` must be valid; all addresses marked usable
    /// must actually be usable.
    unsafe fn init(memory_map: &'static MemoryMap) -> Self {
        Self { memory_map, next: 0 }
    }

    fn usable_frames(&self) -> impl Iterator<Item = PhysFrame> {
        self.memory_map.iter()
            .filter(|region| region.region_type == MemoryRegionType::Usable)
            .map(|region| region.range.start_addr()..region.range.end_addr())
            .flat_map(|region| region.step_by(PAGE_SIZE))
            .map(|address| PhysFrame::containing_address(PhysAddr::new(address)))
    }
}

// TODO: is this needed for how Spinel's amd64 memory subsystem works?
unsafe impl FrameAllocator<Size4KiB> for PageFrameAllocator {
    fn allocate_frame(&mut self) -> Option<PhysFrame<Size4KiB>> {
        let res = self.usable_frames().nth(self.next);
        self.next += 1;
        res
    }
}

/// Initializes the physical memory system.
///
/// ## Safety
/// The `MemoryMap` must be valid; all addresses marked usable
/// must actually be usable.
///
/// ## Panics
/// Panics if this function has already been called.
pub unsafe fn init(memory_map: &'static MemoryMap) {
    let mut alloc = PAGE_FRAME_ALLOCATOR.lock();
    if alloc.is_some() {
        panic!("Tried to initialize the physical memory system twice");
    }
    *alloc = Some(PageFrameAllocator::init(memory_map));
}

/// Allocate an unused physical page frame.
///
/// ## Panics
/// Panics if `init` hasn't been called or if the frame cannot be allocated.
pub fn allocate_frame() -> PhysFrame<Size4KiB> {
    let mut alloc = PAGE_FRAME_ALLOCATOR.lock();
    let alloc = alloc.as_mut().unwrap();
    alloc.allocate_frame().unwrap()
}
