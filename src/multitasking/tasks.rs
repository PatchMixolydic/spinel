use alloc::string::String;
use spin::Mutex;

use crate::arch::multitasking::arch_tasks::{switch_tasks, ArchTask};
use crate::central::id_allocator::IDAllocator;
use crate::multitasking::scheduler::Priority;
use crate::println;

static TASK_ID_ALLOCATOR: Mutex<IDAllocator> = Mutex::new(IDAllocator::new());

/// Thread or process ID.
pub type TaskId = u32;

#[derive(Clone, Copy, Debug)]
pub enum ThreadState {
    Idle,
    Active,
    Asleep,
    Blocked,
    Dead,
}

#[derive(Clone, Debug)]
pub struct Thread {
    /// ID of the parent process.
    process_id: TaskId,
    thread_id: TaskId,
    pub state: ThreadState,
    pub last_scheduled_time: u64,
    pub priority: Priority,
    underlying_task: ArchTask,
}

impl Thread {
    fn new(
        process_id: TaskId,
        instruction_pointer: usize,
        user_mode: bool,
        priority: Priority,
    ) -> Self {
        println!("new thread new thread brand new never before seen");
        let thread_id = TASK_ID_ALLOCATOR.lock().allocate();
        println!("id is {}", thread_id);
        let underlying_task = ArchTask::new(instruction_pointer, user_mode);
        println!("Underlying task: {:#?}", underlying_task);
        Self {
            process_id,
            thread_id,
            state: ThreadState::Idle,
            last_scheduled_time: 0,
            priority,
            underlying_task,
        }
    }

    /// Switch to another task from this one. Convenience wrapper for
    /// `arch::multitasking::arch_tasks::switch_tasks`.
    ///
    /// ## Safety
    /// This function is unsafe because the underlying implementation may
    /// swap out memory mappings and/or the stack, which may lead to trouble
    /// if this thread's ArchTask is invalid!
    pub unsafe fn switch_to_this_from(&mut self, old_thread: Option<&mut Thread>) {
        println!("Switch from {:#X?} to {:#X?}", old_thread, self);
        let old_task = match old_thread {
            Some(thread) => Some(&mut thread.underlying_task),
            None => None,
        };
        switch_tasks(old_task, &mut self.underlying_task);
    }

    pub fn process_id(&self) -> TaskId {
        self.process_id
    }

    pub fn thread_id(&self) -> TaskId {
        self.thread_id
    }
}

#[derive(Debug)]
pub struct Process {
    process_id: TaskId,
    name: String,
    user_mode: bool,
    // uid: user id type,
    // gid: group id type,
    // working_directory: some path type,
    // executable_path: some path type,
    // file_descriptors: ?,
    // root_directory: some path type,
}

impl Process {
    pub fn new(name: impl Into<String>, user_mode: bool) -> Self {
        let process_id = TASK_ID_ALLOCATOR.lock().allocate();
        Self {
            process_id,
            name: name.into(),
            user_mode,
        }
    }

    pub fn spawn_thread(&self, instruction_pointer: usize, priority: Priority) -> Thread {
        println!(
            "Spawning new thread for process {} ({}): IP {:#X} priority {:?}",
            self.process_id, self.name, instruction_pointer, priority
        );
        Thread::new(
            self.process_id,
            instruction_pointer,
            self.user_mode,
            priority,
        )
    }

    pub fn name(&self) -> &str {
        &self.name
    }

    pub fn process_id(&self) -> TaskId {
        self.process_id
    }

    pub fn is_user_mode(&self) -> bool {
        self.user_mode
    }
}
