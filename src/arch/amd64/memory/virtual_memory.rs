use spin::Mutex;
use x86_64::registers::control::{Cr2, Cr3};
use x86_64::structures::idt::{InterruptStackFrame, PageFaultErrorCode};
use x86_64::structures::paging::{
    // Rename to avoid confusion w/ the arch-generic MapError
    mapper::MapToError as MapToErrorX86_64,
    Mapper,
    OffsetPageTable,
    Page,
    PageTable,
    PageTableFlags,
    PhysFrame,
    Size4KiB,
};
use x86_64::{PhysAddr, VirtAddr};

use crate::arch::amd64::central::idt::IDT;
use crate::arch::amd64::memory::physical_memory::{allocate_frame, PAGE_FRAME_ALLOCATOR};
use crate::arch::amd64::memory::PHYSICAL_MEMORY_OFFSET;
use crate::arch::memory::MapError;
use crate::println;

static TOP_LEVEL_PAGE_MAP: Mutex<Option<OffsetPageTable>> = Mutex::new(None);

/// Initializes the virtual memory system.
///
/// ## Safety
/// This function assumes that physical memory is identity mapped at
/// the offset into virtual memory.
///
/// ## Panics
/// This function panics if the virtual memory system has already
/// been set up.
pub unsafe fn init() {
    {
        let mut idt = IDT.lock();
        idt.page_fault.set_handler_fn(page_fault_handler);
        // Relinquish the lock early
        // This initialization code will probably only ever be run
        // single-threaded and before multitasking is initialized,
        // but this is just polite :P
    }

    let mut top_map = TOP_LEVEL_PAGE_MAP.lock();
    if top_map.is_some() {
        panic!("Tried to initialize the virtual memory system twice");
    }
    *top_map = Some(OffsetPageTable::new(
        active_top_level_page_map(),
        VirtAddr::new(PHYSICAL_MEMORY_OFFSET),
    ));
}

/// Behind-the-scenes function that maps a given virtual address to a given physical frame.
/// Used to DRY up the logic shared by `map_virtual_to_physical` and `map_virtual`.
fn map_virtual_to_physical_frame(
    virtual_address: VirtAddr,
    physical_frame: PhysFrame,
    flags: PageTableFlags,
) -> Result<(), MapError> {
    let page = Page::containing_address(virtual_address);

    // TODO: this seems like it could be DRYed, but MutexGuards are a concern
    let mut map = TOP_LEVEL_PAGE_MAP.lock();
    let map = map.as_mut().unwrap();
    let mut page_frame_alloc = PAGE_FRAME_ALLOCATOR.lock();
    let page_frame_alloc = page_frame_alloc.as_mut().unwrap();

    let res = unsafe { map.map_to(page, physical_frame, flags, page_frame_alloc) };

    if let Err(e) = res {
        match e {
            MapToErrorX86_64::FrameAllocationFailed => {
                // oh no
                panic!(
                    "Couldn't map {:?} to {:?} because a frame allocation failed",
                    virtual_address,
                    physical_frame.start_address()
                );
            }
            MapToErrorX86_64::PageAlreadyMapped(_) | MapToErrorX86_64::ParentEntryHugePage => {
                // Page was already mapped!
                return Err(MapError::AlreadyMapped);
            }
        }
    }

    // Flush the translation lookaside buffer
    res.unwrap().flush();
    Ok(())
}

/// Map the page containing the given virtual address
/// to a specific physical address. The backing page will
/// be assigned immediately.
///
/// ## Panics
/// Panics on failure to map the given virtual address.
#[inline]
pub fn map_virtual_to_physical(
    virtual_address: VirtAddr,
    physical_address: PhysAddr,
) -> Result<(), MapError> {
    map_virtual_to_physical_frame(
        virtual_address,
        PhysFrame::containing_address(physical_address),
        PageTableFlags::WRITABLE | PageTableFlags::PRESENT,
    )
}

/// Map the page containing the given virtual address
/// to some unused physical address unlazily.
/// It will be given a backing physical frame immediately.
///
/// ## Panics
/// Panics on failure to map the given virtual address.
/// Also panics if the physical memory system is not initialized
/// or if a new physical page frame could not be allocated.
#[inline]
pub fn map_virtual_address_unlazily(virtual_address: usize) -> Result<(), MapError> {
    map_virtual_to_physical_frame(
        VirtAddr::new(virtual_address as u64),
        allocate_frame(),
        PageTableFlags::WRITABLE | PageTableFlags::PRESENT,
    )
}

/// Map the page containing the given virtual address lazily.
/// It will be given a backing physical frame upon access.
///
/// ## Panics
/// Panics on failure to map the given virtual address.
/// Also panics if the physical memory system is not initialized
/// or if a new physical page frame could not be allocated.
#[inline]
pub fn map_virtual_address(virtual_address: usize) -> Result<(), MapError> {
    // PageTableFlags::BIT_9 is used to inform the virtual memory
    // system that this page was lazily mapped and does not have
    // a backing yet.
    map_virtual_to_physical_frame(
        VirtAddr::new(virtual_address as u64),
        PhysFrame::containing_address(PhysAddr::zero()),
        PageTableFlags::WRITABLE | PageTableFlags::BIT_9,
    )
}

/// Map all pages from start_address to end_address (inclusive)
/// unlazily (should be lazily, but lazy allocation isn't ready!)
///
/// If a page was already mapped, then it is skipped.
///
/// ## Panics
/// Panics whenever map_virtual_address_unlazily would panic.
/// Asserts in debug mode if the start address is less than the
/// end address.
#[inline]
pub fn map_range(start_address: usize, end_address: usize) -> Result<(), MapError> {
    debug_assert!(start_address < end_address);
    let range = Page::<Size4KiB>::range_inclusive(
        Page::containing_address(VirtAddr::new(start_address as u64)),
        Page::containing_address(VirtAddr::new(end_address as u64)),
    );

    for page in range {
        match map_virtual_address_unlazily(page.start_address().as_u64() as usize) {
            Ok(_) => (),
            Err(MapError::AlreadyMapped) => (),
            Err(e) => return Err(e),
        }
    }

    Ok(())
}

extern "x86-interrupt" fn page_fault_handler(
    stack_frame: InterruptStackFrame,
    error_code: PageFaultErrorCode,
) {
    let fault_address = Cr2::read();
    let maybe_page_map = TOP_LEVEL_PAGE_MAP.try_lock();
    if maybe_page_map.is_none() {
        println!("Page fault handler couldn't lock the page map");
    } else {
        // let page_map = maybe_page_map.unwrap().as_mut().unwrap();
        // Check if BIT_9 is set, and if so, allocate a backing page
        // TODO: how do I get the flags?
        println!("Warning: lazy allocation isn't implemented, this might be an issue");
    }

    panic!(
        "Page fault\nCR2: {:?}\nError flags: {:?}\nStack frame: {:#?}",
        fault_address, error_code, stack_frame
    );
}

/// Gets the currently active top level page map,
/// given an offset into virtual memory where physical memory is
/// identity mapped.
///
/// ## Safety
/// This function assumes that the given `PHYSICAL_MEMORY_OFFSET`
/// is indeed where physical memory is identity mapped.
unsafe fn active_top_level_page_map() -> &'static mut PageTable {
    let (top_page_map, _) = Cr3::read();
    let physical_addr = top_page_map.start_address();
    let virtual_addr = VirtAddr::new(PHYSICAL_MEMORY_OFFSET) + physical_addr.as_u64();
    let page_map_ptr: *mut PageTable = virtual_addr.as_mut_ptr();
    &mut *page_map_ptr
}
