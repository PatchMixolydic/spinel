use alloc::boxed::Box;
use alloc::collections::VecDeque;
use alloc::vec::Vec;
use core::sync::atomic::spin_loop_hint;
use lazy_static::lazy_static;
use spin::Mutex;

use crate::arch::interrupts::enable_interrupts;
use crate::devices::timer::{register_timer, TICKS_PER_MILLISECOND, ticks_since_boot};
use crate::multitasking::tasks::{Process, TaskId, Thread, ThreadState};
use crate::println;

// TODO: For SMP, the current process list and list of run queues could use
// Vec (CPUs can be hotplugged)

const NORMAL_TIMESLICE: u64 = 20 * TICKS_PER_MILLISECOND;
const LOW_PRIORITY_TIMESLICE: u64 = 10 * TICKS_PER_MILLISECOND;
const REALTIME_TIMESLICE: u64 = 40 * TICKS_PER_MILLISECOND;

static PROCESSES: Mutex<Vec<Process>> = Mutex::new(Vec::new());
static CURRENT_THREAD: Mutex<Option<Thread>> = Mutex::new(None);

lazy_static! {
    static ref RUN_QUEUES: Mutex<RunQueues> = Mutex::new(RunQueues::default());
}

#[derive(Clone, Copy, Debug, PartialEq)]
#[repr(usize)]
pub enum Priority {
    Abysmal,
    UserLow,
    KernelLow,
    UserNormal,
    KernelNormal,
    UserHigh,
    KernelHigh,
    Realtime,
    NumPriorities
}

type RunQueues = [VecDeque<Thread>; Priority::NumPriorities as usize];

/// Idle.
///
/// The most exquisite function in the entire codebase.
///
/// Just sits there, waiting for something exciting to happen.
///
/// Art imitates life.
///
/// ## Safety
/// If this function you decide to run
///
/// Surely you will get nothing done
///
/// It is completely safe to call
///
/// For it won't do a thing at all
fn idle() {
    println!("Idle thread");
    loop {
        enable_interrupts();
        spin_loop_hint();
    }
}

/// Starts the scheduler.
/// Note that this will start scheduling processes immediately,
/// so you will probably want to create any bootstrap processes
/// before calling this.
pub fn init() {
    println!("Scheduler start!");
    unsafe {
        println!("Add idle proces");
        add_process(Process::new("idle", false), idle as usize, Priority::Abysmal);
    }
    println!("Switch!");
    next_task();
}

fn timeslice_for_priority(priority: Priority) -> u64 {
    assert!(priority != Priority::NumPriorities);
    println!("Resolving timeslice for {:?}", priority);
    match priority {
        Priority::Abysmal | Priority::KernelLow | Priority::UserLow => LOW_PRIORITY_TIMESLICE,

        Priority::UserNormal | Priority::KernelNormal |
            Priority::UserHigh | Priority::KernelHigh => NORMAL_TIMESLICE,

        Priority::Realtime => REALTIME_TIMESLICE,

        // SAFETY: this is actually checked by the above assertion, so
        // we should panic before UB happens
        Priority::NumPriorities => unsafe { core::hint::unreachable_unchecked() }
    }
}

pub fn next_task() {
    println!("Time to switch!");
    let mut run_queues = RUN_QUEUES.try_lock().unwrap();
    let mut current_thread = CURRENT_THREAD.try_lock().unwrap();
    let old_thread = current_thread.take();
    println!("Old thread: {:#?}", old_thread);

    println!("Traverse priority runqueues");
    for priority in (0..Priority::NumPriorities as usize).rev() {
        println!("  {:?}", priority);
        if run_queues[priority].is_empty() {
            println!("    Empty");
            continue;
        }
        *current_thread = Some(run_queues[priority].pop_front().unwrap());
        println!("    OK, got {:#?}", *current_thread);
    }

    let old_thread = old_thread.and_then(|mut thread| {
        println!("Store old thread");
        thread.state = ThreadState::Idle;
        let priority = thread.priority;
        run_queues[priority as usize].push_back(thread);
        run_queues[priority as usize].back_mut()
    });


    if current_thread.is_none() {
        // That shouldn't be, the idle thread should be scheduled!
        panic!("No threads found, not even the idle thread!");
    }

    let next_thread = current_thread.as_mut().unwrap();
    println!("Set next thread state");
    next_thread.state = ThreadState::Active;
    next_thread.last_scheduled_time = ticks_since_boot();
    println!("Thread scheduled at {}", next_thread.last_scheduled_time);
    register_timer(
        timeslice_for_priority(next_thread.priority), true, Box::new(next_task)
    ).unwrap();
    println!("Timer registered");
    unsafe {
        println!("Switch!");
        next_thread.switch_to_this_from(old_thread);
    }
}

/// Add a process to the scheduler, creating a thread for it to run.
///
/// ## Safety
/// This function assumes that the instruction_pointer points to valid
/// machine code in executable memory.
pub unsafe fn add_process(process: Process, instruction_pointer: usize, priority: Priority) -> TaskId {
    println!("Add process {:#?} with IP {:#010X} and priority {:?}", process, instruction_pointer, priority);
    assert!(priority != Priority::NumPriorities);
    println!("Spawn thread...");
    let thread = process.spawn_thread(instruction_pointer, priority);
    println!("Got {:#?}", thread);
    let res = thread.thread_id();
    println!("Pushing process");
    PROCESSES.lock().push(process);
    println!("OK, enqueuing thread");
    enqueue_thread(thread, priority);
    println!("OK");
    res
}

/// Spawn a new thread for the currently running process.
///
/// ## Safety
/// This function assumes that the instruction_pointer points to valid
/// machine code in executable memory.
pub unsafe fn spawn_thread_from_active(instruction_pointer: usize, priority: Priority) {
    let current_process_id = {
        let current_thread_guard = CURRENT_THREAD.lock();
        let current_thread = current_thread_guard.as_ref()
            .unwrap_or_else(|| panic!("Tried to spawn a thread from the active process with no active process"));
        current_thread.process_id()
    };

    let processes = PROCESSES.lock();
    let thread = processes[current_process_id as usize].spawn_thread(instruction_pointer, priority);
    enqueue_thread(thread, priority);
}

/// Enqueue a thread in the relevant runqueue.
///
/// ## Safety
/// This function assumes that the instruction_pointer points to valid
/// machine code in executable memory.
unsafe fn enqueue_thread(thread: Thread, priority: Priority) {
    assert!(priority != Priority::NumPriorities);
    let mut run_queues = RUN_QUEUES.lock();
    run_queues[priority as usize].push_back(thread);
}