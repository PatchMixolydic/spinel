use alloc::alloc::{GlobalAlloc, Layout};
use core::mem::{align_of, size_of};
use core::ptr::null_mut;
use spin::{Mutex, MutexGuard};

use crate::arch::memory::{
    HEAP_SIZE,
    HEAP_START,
    map_range,
    map_virtual_address_unlazily
};

#[global_allocator]
static ALLOCATOR: AllocMutex = AllocMutex::new(KernelAllocator::new());

#[repr(C)]
pub struct KernelAllocatorNode {
    next_free: Option<&'static mut KernelAllocatorNode>,
    size: usize,
}

impl KernelAllocatorNode {
    const fn new(size: usize) -> Self {
        Self { next_free: None, size }
    }

    fn start_address(&self) -> usize {
        self as *const Self as usize
    }

    fn end_address(&self) -> usize {
        self.start_address() + self.size
    }
}

struct KernelAllocator {
    free_list: KernelAllocatorNode,
    initialized: bool
}

impl KernelAllocator {
    const fn new() -> Self {
        Self { free_list: KernelAllocatorNode::new(0), initialized: false }
    }

    fn init(&mut self) {
        assert!(!self.initialized);
        self.initialized = true;
        // Freeing the heap requires that the first page of the heap is writable.
        // Any errors here are a hard error.
        map_virtual_address_unlazily(HEAP_START).unwrap();
        unsafe {
            self.free_region(HEAP_START, HEAP_SIZE);
        }
    }

    /// Add a region onto the free list.
    ///
    /// ## Safety
    /// This function is unsafe because this function partially assumes that
    /// address and size are perfectly valid heap values.
    unsafe fn free_region(&mut self, address: usize, size: usize) {
        assert!(size >= size_of::<KernelAllocatorNode>());

        let mut node = KernelAllocatorNode::new(size);
        node.next_free = self.free_list.next_free.take();
        let node_ptr = address as *mut KernelAllocatorNode;
        node_ptr.write(node);
        self.free_list.next_free = Some(&mut *node_ptr);
    }

    fn find_region(&mut self, size: usize, alignment: usize)
    -> Option<(&'static mut KernelAllocatorNode, usize)> {
        let mut current = &mut self.free_list;
        while let Some(ref mut region) = current.next_free {
            if let Ok(alloc_start) = Self::region_to_allocation(&region, size, alignment) {
                let next = region.next_free.take();
                let res = Some((current.next_free.take().unwrap(), alloc_start));
                current.next_free = next;
                return res;
            } else {
                current = current.next_free.as_mut().unwrap();
            }
        }

        None
    }

    fn region_to_allocation(region: &KernelAllocatorNode, size: usize, alignment: usize)
    -> Result<usize, ()> {
        let start = align_up(region.start_address(), alignment);
        let end = start.checked_add(size).ok_or(())?;

        if end > region.end_address() {
            // Region too small
            return Err(());
        }

        let excess = region.end_address() - end;
        if excess > 0 && excess < size_of::<KernelAllocatorNode>() {
            // Rest of region is too small to hold a KernelAllocatorNode
            return Err(());
        }

        Ok(start)
    }

    fn size_align(layout: Layout) -> (usize, usize) {
        let layout = layout
            .align_to(align_of::<KernelAllocatorNode>())
            .expect("adjusting alignment failed")
            .pad_to_align();
        let size = layout.size().max(size_of::<KernelAllocatorNode>());
        (size, layout.align())
    }
}

// Wrapper for mutex so GlobalAlloc can be implemented on it
pub struct AllocMutex {
    inner: Mutex<KernelAllocator>
}

impl AllocMutex {
    const fn new(inner: KernelAllocator) -> Self {
        Self { inner: Mutex::new(inner) }
    }

    fn lock(&self) -> MutexGuard<KernelAllocator> {
        self.inner.lock()
    }
}

unsafe impl GlobalAlloc for AllocMutex {
    unsafe fn alloc(&self, layout: Layout) -> *mut u8 {
        let (size, alignment) = KernelAllocator::size_align(layout);
        let mut allocator = self.lock();

        if let Some((region, allocation_start)) = allocator.find_region(size, alignment) {
            let allocation_end = allocation_start.checked_add(size).expect("overflow");
            let excess_size = region.end_address() - allocation_end;
            if excess_size > 0 {
                allocator.free_region(allocation_end, excess_size);
            }
            map_range(allocation_start, allocation_end).unwrap();
            allocation_start as *mut u8
        } else {
            null_mut()
        }
    }

    unsafe fn dealloc(&self, ptr: *mut u8, layout: Layout) {
        let (size, _) = KernelAllocator::size_align(layout);
        self.lock().free_region(ptr as usize, size);
    }
}

#[alloc_error_handler]
fn alloc_error_handler(layout: Layout) -> ! {
    panic!("Allocation failure for {:?}", layout);
}

pub fn init() {
    ALLOCATOR.lock().init();
}

fn align_up(address: usize, alignment: usize) -> usize {
    (address + alignment - 1) & !(alignment - 1)
}
