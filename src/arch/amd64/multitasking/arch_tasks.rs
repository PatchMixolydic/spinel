use alloc::boxed::Box;
use core::{mem::size_of, ops::Drop};
use x86_64::{registers::control::Cr3Flags, structures::paging::frame::PhysFrame};

use crate::{
    arch::amd64::central::{arch_info::PAGE_SIZE, gdt_tss::GDT},
    println,
};

// Number of u64s that comprise a kernel stack
const KERNEL_STACK_NUM_U64S: usize = PAGE_SIZE / size_of::<u64>();

// TODO: don't derive `Clone`
#[derive(Clone, Debug)]
#[repr(C)]
pub struct ArchTask {
    /// Value of the stack pointer on last switch
    kernel_stack_pointer: u64,
    /// The top of the kernel stack
    kernel_stack_top_pointer: u64,
    /// Value of `rbp` on last switch
    current_base_pointer: u64,
    /// Value of `rip` on last switch
    current_instruction_pointer: u64,
    page_map: (PhysFrame, Cr3Flags),
}

impl ArchTask {
    pub fn new(instruction_pointer: usize, user_mode: bool) -> Self {
        println!(
            "New arch task: IP {:#010X} user_mode {}",
            instruction_pointer, user_mode
        );
        let mut kernel_stack = Box::new([0u64; KERNEL_STACK_NUM_U64S]);
        println!("Kernel stack {:?}", kernel_stack.as_ptr());
        // Index to the bottom of the stack
        let mut stack_idx: usize = KERNEL_STACK_NUM_U64S - 1;
        println!("Starting stack_idx {}", stack_idx);

        // Setting up the stack...
        // Stack segment
        println!("Putting stack segment at {}", stack_idx);
        kernel_stack[stack_idx] = 0;
        stack_idx -= 1;
        // Stack pointer
        // This requires unboxing kernel_stack if user_mode is false,
        // so this will be taken care of later
        println!("Will put RSP at {}", stack_idx);
        let stack_rsp_idx = stack_idx;
        stack_idx -= 1;
        // Flags
        println!("Putting flags at {}", stack_idx);
        // enable interrupts after `iret`
        kernel_stack[stack_idx] = 0x0202;
        stack_idx -= 1;
        // Code segment
        println!("Putting code segment at {}", stack_idx);
        kernel_stack[stack_idx] = if user_mode {
            GDT.1.user_code().0 as u64
        } else {
            GDT.1.kernel_code().0 as u64
        };
        stack_idx -= 1;
        // Instruction pointer for interrupt stack frame
        println!("Putting RIP for interrupt frame at {}", stack_idx);
        kernel_stack[stack_idx] = instruction_pointer as u64;

        // Now we can get the stack pointer
        // Note that the code doesn't just initialize kernel_stack_pointer
        // and kernel_stack_top_pointer, it also writes the stack pointer into the stack
        // SAFETY: this code performs pointer arithmetic and writes to these pointers
        // Since the indices used here had to have passed the bounds checks from
        // the previous uses of kernel_stack, this should work out
        let (kernel_stack_pointer, kernel_stack_top_pointer) = unsafe {
            println!("Open up the Box");
            let kernel_stack_top_pointer = Box::into_raw(kernel_stack) as *mut u64;
            println!("Inside was a {:?}!", kernel_stack_top_pointer);
            // Here's where the stack pointer should go...
            let stack_pointer_loc = kernel_stack_top_pointer.add(stack_rsp_idx);
            println!("RSP will be put in at {:?}", stack_pointer_loc);
            // ... and here's the real kernel stack pointer.
            let kernel_stack_pointer = kernel_stack_top_pointer.add(stack_idx) as u64;
            println!("The actual value of RSP will be {:?}", kernel_stack_pointer);

            if user_mode {
                // Of course, user mode doesn't need to
                // jump through hoops to write the stack pointer
                // into the stack since the user mode stack is
                // different from the kernel stack
                panic!("Please add usermode stack creation")
            } else {
                println!(
                    "Writing KSP {:X?} to {:?}",
                    kernel_stack_pointer, stack_pointer_loc
                );
                stack_pointer_loc.write(kernel_stack_pointer);
            }

            (kernel_stack_pointer, kernel_stack_top_pointer as u64)
        };

        println!("return return return return return !!!!!!!!111");

        // This is architecture-specific code, so we know that pointers are 64-bit
        #[allow(clippy::fn_to_numeric_cast)]
        Self {
            kernel_stack_pointer,
            kernel_stack_top_pointer,
            current_base_pointer: kernel_stack_top_pointer,
            current_instruction_pointer: process_continue as u64,
            page_map: x86_64::registers::control::Cr3::read(),
        }
    }
}

impl Drop for ArchTask {
    fn drop(&mut self) {
        // SAFETY: this is unsafe because just stuffing any old raw address into a Box
        // can cause UB on dropping it (much like freeing a non-malloced address in C{,++}).
        // Fortunately, this address was created from a Box, so we know this was
        // heap allocated.
        println!("Boxing it all up and dropping it...");
        unsafe {
            // The box will free the allocation for the task's kernel stack when dropped
            drop(Box::from_raw(self.kernel_stack_top_pointer as *mut u64));
        }
        println!("OK");
    }
}

/// Switch from one task to the next.
/// If there is no previous task, just switches
/// to the next one without saving the current
/// state.
///
/// ## Safety
/// Assembly could violate memory safety.
/// This reads off the old task's stack pointer,
/// and then swaps in the new task. This requires
/// new_task to have a valid stack pointer and
/// page map, or else things will go horribly wrong.
pub unsafe extern "C" fn switch_tasks(
    maybe_old_task: Option<&mut ArchTask>,
    new_task: &mut ArchTask,
) {
    if let Some(old_task) = maybe_old_task {
        // TODO: get EIP
        // TODO: CR3
        asm!(
            "
            mov {old_stack_ptr}, rsp
            mov {old_base_ptr}, rbp
            ",
            old_stack_ptr = out(reg) old_task.kernel_stack_pointer,
            old_base_ptr = out(reg) old_task.current_base_pointer,
        );
        old_task.current_instruction_pointer = process_continue as u64;
    }
    asm!(
        "
        mov rcx, {new_instruction_ptr}
        mov rsp, {new_stack_ptr}
        mov ss, {stack_segment:x}
        mov rbp, {new_base_ptr}
        jmp rcx
        ",
        new_instruction_ptr = in(reg) new_task.current_instruction_pointer,
        new_stack_ptr = in(reg) new_task.kernel_stack_pointer,
        // Always the kernel stack pointer
        stack_segment = in(reg) GDT.1.kernel_data().0,
        new_base_ptr = in(reg) new_task.current_base_pointer,
        // rcx is clobbered
        out("rcx") _
    );
}

/// The code all threads run when resuming
/// execution.
///
/// This is just a trampoline to pretend like
/// we're returning from IRQ0 specifically. You probably
/// don't want to call this function directly.
///
/// ## Safety
/// This function is unsafe because it assumes
/// that when it's called, an interrupt stack frame
/// is the next thing on the stack.
#[naked]
unsafe extern "C" fn process_continue() {
    asm!(
        "push ax",
        "mov al, 0x20",
        "out 0x20, al",
        "pop ax",
        "iretq",
        options(noreturn)
    );
}
