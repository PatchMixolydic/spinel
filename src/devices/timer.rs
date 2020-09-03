use alloc::boxed::Box;
use alloc::vec::Vec;
use spin::Mutex;

pub const TICKS_PER_NANOSECOND: u64 = 1; // 1 tick = 1 us
pub const TICKS_PER_MILLISECOND: u64 = TICKS_PER_NANOSECOND * 1000;
pub const TICKS_PER_SECOND: u64 = TICKS_PER_MILLISECOND * 1000;

static TIMER_STATE: Mutex<Option<TimerState>> = Mutex::new(None);

type TimerCallback = dyn FnMut() + 'static + Send;

#[derive(Clone, Copy, Debug)]
pub enum TimerError {
    CouldntLockState
}

struct Timer {
    id: u128,
    next_time: u64,
    duration: u64,
    callback: Box<TimerCallback>,
    one_shot: bool
}

impl PartialEq for Timer {
    fn eq(&self, other: &Self) -> bool {
        #[cfg(debug_assertions)] {
            if self.id == other.id {
                debug_assert_eq!(self.next_time, other.next_time);
                debug_assert_eq!(self.duration, other.duration);
                debug_assert_eq!(self.one_shot, other.one_shot);
            }
        }
        self.id == other.id
    }
}

struct TimerState {
    ticks_since_boot: u64,
    ticks_per_clock_cycle: u64,
    timers: Vec<Timer>,
    // TODO: think of something better!
    next_timer_id: u128
}

impl TimerState {
    const fn new(frequency: u64) -> Self {
        TimerState {
            ticks_since_boot: 0,
            ticks_per_clock_cycle: TICKS_PER_SECOND / frequency,
            timers: Vec::new(),
            next_timer_id: 0
        }
    }

    fn next_id(&mut self) -> u128 {
        // TODO: do something smarter
        self.next_timer_id = match self.next_timer_id.checked_add(1) {
            Some(val) => val,
            None => panic!("Too many timers allocated; next_timer_id overflowed")
        };
        self.next_timer_id - 1
    }
}

/// Initialize the timer subsystem.
/// This should most likely be called when initializing the
/// architecture's backing timer device.
/// This is the only function in this submodule than can be called
/// before the memory allocator is initialized.
pub fn init(frequency: u64) {
    let mut state = match TIMER_STATE.try_lock() {
        Some(state) => state,
        None => panic!("Couldn't lock the timer state while initializing")
    };

    if state.is_some() {
        panic!("Attempted to initialize timer subsystem twice");
    }

    *state = Some(TimerState::new(frequency));
}

pub fn ticks_since_boot() -> u64 {
    match &*TIMER_STATE.lock() {
        Some(state) => state.ticks_since_boot,
        None => panic!("Tried to get ticks since boot before initializing timer")
    }
}

/// Tick the kernel timer. If it's time for any registered timers to fire,
/// fire them.
/// It is recommended that only the architecture-specific timer calls
/// this function.
///
/// ## Panics
/// Panics if the timer system is not initialized or if the count of
/// ticks since boot overflows
pub fn tick() {
    let mut state_guard = match TIMER_STATE.try_lock() {
        Some(guard) => guard,
        None => return
    };
    let mut state = match &mut *state_guard {
        Some(s) => s,
        None => panic!("Tried to tick timer before initializing it")
    };

    // TODO: should this be a hard error?
    state.ticks_since_boot = match state.ticks_since_boot.checked_add(state.ticks_per_clock_cycle) {
        Some(res) => res,
        None => panic!("ticks_since_boot overflowed")
    };

    for timer in &mut state.timers {
        if timer.next_time <= state.ticks_since_boot {
            (timer.callback)();
            if timer.one_shot {
                // TODO: actually remove from the list
                // I can't think of a sound way to do this rn
                // FIXME: HACKFIX: this is not very good!
                // Better than memory corruption or UB, but still...
                timer.next_time = u64::MAX;
            } else {
                timer.next_time += timer.duration;
            }
        }
    }
}

/// Register a timer to wait for at least the given duration and then call a callback.
/// Returns an id wrapped in `Ok` on success.
///
/// ## Panics
/// Panics when called before the timer system has been initialized.
pub fn register_timer(duration: u64, one_shot: bool, callback: Box<TimerCallback>) -> Result<u128, TimerError> {
    let mut state_guard = match TIMER_STATE.try_lock() {
        Some(state) => state,
        None => return Err(TimerError::CouldntLockState)
    };

    let state = match &mut *state_guard {
        Some(s) => s,
        None => panic!("Called register_timer before timer system was initialized")
    };

    let id = state.next_id();
    let timer = Timer {
        id,
        duration,
        next_time: duration + state.ticks_since_boot,
        one_shot,
        callback
    };
    state.timers.push(timer);

    Ok(id)
}
