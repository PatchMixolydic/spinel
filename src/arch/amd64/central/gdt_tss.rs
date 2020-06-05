use lazy_static::lazy_static;
use x86_64::instructions::segmentation::set_cs;
use x86_64::instructions::tables::load_tss;
use x86_64::structures::gdt::{Descriptor, GlobalDescriptorTable, SegmentSelector};
use x86_64::structures::tss::TaskStateSegment;
use x86_64::VirtAddr;

use crate::arch::amd64::central::arch_info::PAGE_SIZE;

/// A double fault may result from a stack overflow hitting the guard page
/// at the stack's top. To prevent this from turning into a triple fault
/// when the CPU tries to push the exception stack frame, a clean stack
/// is used for double faults.
pub const DOUBLE_FAULT_STACK: u16 = 0;

lazy_static! {
    static ref TSS: TaskStateSegment = {
        let mut tss = TaskStateSegment::new();
        tss.interrupt_stack_table[usize::from(DOUBLE_FAULT_STACK)] = {
            static mut STACK: [u8; PAGE_SIZE] = [0; PAGE_SIZE];
            VirtAddr::from_ptr(unsafe { &STACK }) + PAGE_SIZE
        };
        tss
    };
}

lazy_static! {
    static ref GDT: (GlobalDescriptorTable, GDTSelectors) = {
        let mut gdt = GlobalDescriptorTable::new();
        let kernel_code = gdt.add_entry(Descriptor::kernel_code_segment());
        let tss = gdt.add_entry(Descriptor::tss_segment(&TSS));
        (gdt, GDTSelectors { kernel_code, tss })
    };
}

struct GDTSelectors {
    kernel_code: SegmentSelector,
    tss: SegmentSelector,
}

pub fn init() {
    GDT.0.load();
    // SAFETY: These write arbitrary values to CPU registers
    unsafe {
        // Set our code segment register to mark that we're in kernel code
        set_cs(GDT.1.kernel_code);
        // The task state segment is important, too
        load_tss(GDT.1.tss);
    }
}
