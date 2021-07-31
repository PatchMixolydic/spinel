use lazy_static::lazy_static;
use spin::Mutex;
use x86_64::structures::idt::{InterruptDescriptorTable, InterruptStackFrame};

use crate::arch::amd64::central::gdt_tss::DOUBLE_FAULT_STACK;
use crate::arch::amd64::devices::pic::IRQ0_INTERRUPT;

pub const NUM_IRQS: u8 = 16;

lazy_static! {
    pub static ref IDT: Mutex<InterruptDescriptorTable> = Mutex::new(InterruptDescriptorTable::new());
}

type IRQHandler = extern "x86-interrupt" fn(InterruptStackFrame);

/// Wrapper for a u8 that enforces that its contents
/// represent a valid IRQ.
///
/// It implements From<u8>, so for pretty much any
/// function that takes an IRQ, you can use `into()`:
/// ```
/// fn foo(irq: IRQ) {
///     // ...
/// }
///
/// foo(1.into());
/// ```
#[repr(transparent)]
pub struct IRQ {
    id: u8,
}

impl IRQ {
    pub fn id(&self) -> u8 {
        self.id
    }
}

impl From<u8> for IRQ {
    fn from(val: u8) -> Self {
        assert!(val < NUM_IRQS);
        IRQ { id: val }
    }
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
        idt.general_protection_fault.set_handler_fn(general_protection_fault_handler);
        idt.load_unsafe();
    }
}

/// Convenience function to register an IRQ handler
pub fn register_irq_handler(irq: IRQ, handler: IRQHandler) {
    // TODO: what should be done if an IRQ already has a handler?
    let mut idt = IDT.lock();
    idt[IRQ0_INTERRUPT + usize::from(irq.id())].set_handler_fn(handler);
}

extern "x86-interrupt" fn double_fault_handler(
    stack_frame: InterruptStackFrame,
    _error_code: u64
) -> ! {
    // Error code is always 0
    panic!("Double fault\nStack frame: {:#?}", stack_frame);
}

extern "x86-interrupt" fn general_protection_fault_handler(
    stack_frame: InterruptStackFrame,
    error_code: u64
) {
    panic!("General protection fault\nError code {}\nStack frame: {:#?}", error_code, stack_frame);
}
