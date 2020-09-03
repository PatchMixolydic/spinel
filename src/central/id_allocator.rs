use alloc::vec::Vec;

use crate::println;

const MAX_ID_INDEX: usize = u32::MAX as usize / core::mem::size_of::<u64>();

/// An ID allocator. It aims to take up little memory by default, only growing
/// its allocation space if it needs to. It can currently hold up to
/// u32::MAX (~4.27 billion) IDs.
pub struct IDAllocator {
    id_bitmaps: Vec<u64>,
    last_index: usize
}

impl IDAllocator {
    pub const fn new() -> Self {
        Self { id_bitmaps: Vec::new(), last_index: 0 }
    }

    pub fn allocate(&mut self) -> u32 {
        let start_idx = self.last_index;
        // Look for the next good bitmap
        println!(
            "Looking in bitmap, starting at index {} ({:#X})",
            self.last_index,
            self.id_bitmaps.get(self.last_index).unwrap_or(&0)
        );
        let mut i = 0;
        let mut bitmap = loop {
            i += 1;
            println!("  Iteration {}", i);
            let bitmap = match self.id_bitmaps.get_mut(self.last_index) {
                Some(x) => *x,
                None => 0
            };
            println!("    Here's our bitmap: {:#X}", bitmap);
            if bitmap == u64::MAX {
                // Oops, this bitmap is full
                println!("    Bitmap full");
                self.last_index += 1;
                if self.last_index > MAX_ID_INDEX {
                    self.last_index = 0
                }
                println!("    Check {}", self.last_index);
                if self.last_index == start_idx {
                    panic!("Out of ids");
                }
            } else {
                // Found one!
                println!("    WoMM");
                break bitmap;
            }
        };

        // Here's the base of the result
        let mut res = (core::mem::size_of::<u64>() * self.last_index) as u32;
        println!("I think the result starts at {}", res);
        // A bitmap for shifting through
        let mut search_bitmap = bitmap;
        // Find the next available bit from the bottom
        for x in 0..64 {
            if search_bitmap & 1 == 0 {
                println!("Free bit: {}", x);
                res += x;
                println!("Gives us a result {}", res);
                bitmap |= 1 << x;
                println!("Bitmap set to {:#X}", bitmap);
                break;
            }
            search_bitmap >>= 1;
        }

        if self.last_index == self.id_bitmaps.len() {
            self.id_bitmaps.push(bitmap);
        } else {
            self.id_bitmaps[self.last_index] = bitmap;
        }
        println!("Here is id_bitmaps: {:?}", self.id_bitmaps);
        res
    }

    pub fn free(&mut self, id: u32) {
        let index = id as usize / core::mem::size_of::<u64>();
        // Of course, integer division means that the decimal place gets dropped
        // Let's use this to get the bit offset
        let bit = id as usize - (index * core::mem::size_of::<u64>());
        self.id_bitmaps[index] |= 1 << bit;
    }
}
