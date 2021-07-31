use lazy_static::lazy_static;
use x86_64::instructions::segmentation::{Segment, CS};
use x86_64::instructions::tables::load_tss;
use x86_64::structures::gdt::{
    Descriptor, DescriptorFlags, GlobalDescriptorTable, SegmentSelector,
};
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
    pub static ref GDT: (GlobalDescriptorTable, GDTSelectors) = {
        let mut gdt = GlobalDescriptorTable::new();
        let kernel_code = gdt.add_entry(Descriptor::kernel_code_segment());
        let kernel_data = gdt.add_entry({
            // User segment in contrast to things like a TSS
            let flags = DescriptorFlags::USER_SEGMENT | DescriptorFlags::PRESENT | DescriptorFlags::WRITABLE;
            Descriptor::UserSegment(flags.bits())
        });
        let user_code = gdt.add_entry(Descriptor::user_code_segment());
        let user_data = gdt.add_entry(Descriptor::user_data_segment());
        let tss = gdt.add_entry(Descriptor::tss_segment(&TSS));
        (gdt, GDTSelectors { kernel_code, kernel_data, user_code, user_data, tss })
    };
}

pub struct GDTSelectors {
    kernel_code: SegmentSelector,
    kernel_data: SegmentSelector,
    user_code: SegmentSelector,
    user_data: SegmentSelector,
    tss: SegmentSelector,
}

impl GDTSelectors {
    pub fn kernel_code(&self) -> &SegmentSelector {
        &self.kernel_code
    }

    pub fn kernel_data(&self) -> &SegmentSelector {
        &self.kernel_data
    }

    pub fn user_code(&self) -> &SegmentSelector {
        &self.user_code
    }

    pub fn user_data(&self) -> &SegmentSelector {
        &self.user_data
    }

    pub fn tss(&self) -> &SegmentSelector {
        &self.tss
    }
}

pub fn init() {
    GDT.0.load();
    // SAFETY: These write arbitrary values to CPU registers
    unsafe {
        // Set our code segment register to mark that we're in kernel code
        CS::set_reg(GDT.1.kernel_code);
        // The task state segment is important, too
        load_tss(GDT.1.tss);
    }
}
