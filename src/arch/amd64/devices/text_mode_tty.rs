use core::fmt;
use core::ptr::{read_volatile, write_volatile};
use lazy_static::lazy_static;
use num_derive::{FromPrimitive, ToPrimitive};
use num_traits::cast::{FromPrimitive, ToPrimitive};
use spin::Mutex;

const SCREEN_COLUMNS: isize = 80;
const SCREEN_ROWS: isize = 25;

lazy_static! {
    pub static ref WRITER: Mutex<Writer> = Mutex::new(Writer::default());
}

#[repr(u8)]
#[derive(Clone, Copy, FromPrimitive, ToPrimitive)]
pub enum VGAColour {
    Black,
    Blue,
    Green,
    Cyan,
    Red,
    Magenta,
    Brown,
    LightGray,
    DarkGray,
    LightBlue,
    LightGreen,
    LightCyan,
    LightRed,
    Pink,
    Yellow,
    White
}

/// Represents the data that makes up a VGA character
#[derive(Clone, Copy)]
struct VGAChar {
    colour: u8,
    character: u8
}

impl VGAChar {
    fn new(fg_colour: VGAColour, bg_colour: VGAColour, character: u8) -> Self {
        let colour = (bg_colour.to_u8().unwrap()) << 4 | fg_colour.to_u8().unwrap();
        Self {colour, character}
    }

    fn fg_colour(&self) -> VGAColour {
        // The set of possible values is constrained to 0x00-0x0F,
        // which matches the range of VGAColour, so this unwrap shouldn't panic
        VGAColour::from_u8(self.colour & 0x0F).unwrap()
    }

    fn bg_colour(&self) -> VGAColour {
        // The set of possible values is constrained to 0x00-0x0F,
        // which matches the range of VGAColour, so this unwrap shouldn't panic
        VGAColour::from_u8((self.colour >> 4) & 0x0F).unwrap()
    }
}

/// Provides access to the VGA text mode buffer.
/// Could be done with a const pointer to mutable data,
/// especially since writes would be unsafe anyway,
/// but I like the association of functions :>
struct VGABuffer {
    chars: *mut u8
}

impl VGABuffer {
    /// Get the VGA text buffer.
    /// It's named "get" instead of "new" because there is only one such buffer.
    fn get() -> Self {
        Self {chars: 0x000B_8000 as *mut u8}
    }

    /// Write a character at the given coordinates
    fn write_at(&mut self, ch: VGAChar, row: isize, column: isize) {
        assert!(row < SCREEN_ROWS && column < SCREEN_COLUMNS);
        // Columns are 2 u8s wide
        let column = column * 2;
        // SAFETY:
        // This block performs a volatile write to the VGA text buffer,
        // which ensures the write will not be optimized out.
        // This block is guarded by a bounds check on the index into the buffer.
        unsafe {
            write_volatile(self.chars.offset(row * SCREEN_COLUMNS * 2 + column), ch.character);
            write_volatile(self.chars.offset(row * SCREEN_COLUMNS * 2 + column + 1), ch.colour);
        }
    }

    /// Read the character at the given coordinates
    fn read_at(&self, row: isize, column: isize) -> VGAChar {
        assert!(row < SCREEN_ROWS && column < SCREEN_COLUMNS);
        let character;
        let colour;
        let column = column * 2;
        // SAFETY:
        // Similar to write_at, this block performs a volatile read from the VGA text buffer,
        // which ensures the read will not be optimized out.
        // This block is guarded by a bounds check on the index into the buffer.
        unsafe {
            character = read_volatile(self.chars.offset(row * SCREEN_COLUMNS * 2 + column));
            colour = read_volatile(self.chars.offset(row * SCREEN_COLUMNS * 2 + column + 1));
        }
        VGAChar {colour, character}
    }
}

pub struct Writer {
    row: isize,
    column: isize,
    fg_colour: VGAColour,
    bg_colour: VGAColour
}

impl Writer {
    fn clear_row(&mut self, row: isize) {
        let space = VGAChar::new(self.fg_colour, self.bg_colour, b' ');
        for column in 0..SCREEN_COLUMNS {
            VGABuffer::get().write_at(space, row, column);
        }
    }

    fn new_line(&mut self) {
        self.column = 0;
        self.row = self.row + 1;
        if self.row >= SCREEN_ROWS {
            self.row = SCREEN_ROWS - 1;
            for row in 1..SCREEN_ROWS {
                for column in 0..SCREEN_COLUMNS {
                    let character = VGABuffer::get().read_at(row, column);
                    VGABuffer::get().write_at(character, row - 1, column);
                }
            }
        }
        self.clear_row(SCREEN_ROWS - 1);
    }

    pub fn write_byte(&mut self, character: u8) {
        match character {
            b'\n' => self.new_line(),
            _ => {
                if self.column >= SCREEN_COLUMNS {
                    self.new_line();
                }
                let character = VGAChar::new(self.fg_colour, self.bg_colour, character);
                VGABuffer::get().write_at(character, self.row, self.column);
                self.column += 1;
            }
        }
    }

    pub fn write_string(&mut self, s: &str) {
        for byte in s.bytes() {
            match byte {
                0x20..=0x7E | b'\n' => self.write_byte(byte),
                _ => ()
            }
        }
    }
}

impl Default for Writer {
    fn default() -> Self {
        Self {row: 0, column: 0, bg_colour: VGAColour::Black, fg_colour: VGAColour::White}
    }
}

impl fmt::Write for Writer {
    fn write_str(&mut self, s: &str) -> Result<(), fmt::Error> {
        self.write_string(s);
        Ok(())
    }
}
