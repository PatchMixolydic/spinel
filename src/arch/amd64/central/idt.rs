use lazy_static::lazy_static;
use x86_64::structures::idt::{InterruptDescriptorTable, InterruptStackFrame};

use crate::arch::amd64::central::gdt_tss::DOUBLE_FAULT_STACK;

lazy_static! {
    static ref IDT: InterruptDescriptorTable = {
        let mut idt = InterruptDescriptorTable::new();
        let double_fault_entry = idt.double_fault.set_handler_fn(double_fault_handler);
        // SAFETY: Setting the IST index can be precarious if an invalid index is used.
        unsafe {
            double_fault_entry.set_stack_index(DOUBLE_FAULT_STACK);
        }
        idt
    };
}

pub fn init() {
    IDT.load();
}

extern "x86-interrupt" fn double_fault_handler(
    stack_frame: &mut InterruptStackFrame,
    _error_code: u64,
) -> ! {
    // Error code is always 0
    panic!("Double fault\nStack frame: {:#?}", stack_frame);
}
