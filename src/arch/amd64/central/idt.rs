use lazy_static::lazy_static;
use spin::Mutex;
use x86_64::structures::idt::{InterruptDescriptorTable, InterruptStackFrame};

use crate::arch::amd64::central::gdt_tss::DOUBLE_FAULT_STACK;

lazy_static! {
    pub static ref IDT: Mutex<InterruptDescriptorTable> = Mutex::new(InterruptDescriptorTable::new());
}

pub fn init() {
    let mut idt = IDT.lock();
    // SAFETY: Setting the interrupt stack can be precarious
    // if the TSS hasn't been set up.
    // InterruptDescriptorTable.load() expects &'static self,
    // but the mutex guard doesn't guarantee this. Fortunately, we can
    // tell the interrupt descriptor table is static since it's contained
    // within the static mutex IDT.
    unsafe {
        idt.double_fault.set_handler_fn(double_fault_handler).set_stack_index(DOUBLE_FAULT_STACK);
        idt.load_unsafe();
    }
}

extern "x86-interrupt" fn double_fault_handler(
    stack_frame: &mut InterruptStackFrame,
    _error_code: u64
) -> ! {
    // Error code is always 0
    panic!("Double fault\nStack frame: {:#?}", stack_frame);
}