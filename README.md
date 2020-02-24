# Spinel

[![Build Status](https://travis-ci.com/Sparkpin/spinel.svg?branch=enhancement%2Frestart)](https://travis-ci.com/Sparkpin/spinel)

*"I'm so excited to meet you!"*

Spinel is an operating system. It was written to allow me to explore the world
of systems programming after trying and failing to wait patiently for it to be
taught to me in university.

Right now, Spinel is in a very prototypical state. It currently only supports x86.
Future plans include x86_64 support, user mode, multitasking, and someday, a GUI.
Other architectures might be supported as well (Motorola 68k, PowerPC, ARM, and RISC-V 
are all likely candidates).

Spinel was originally started in December 2019. That version of Spinel is still available
on another branch. In February 2020, I decided to restart development on Spinel, for a
few reasons:
* I know what I'm doing a bit better, so this might lead to cleaner code
* I wanted to replace the memory management system, which might be quite messy with the
old codebase
* I can use a third party libc instead of writing my own, at least to get started
* Spinel is no longer based on OSDev's "Meaty Skeleton" tutorial (which covers good
practices for storing files, using a simple kernel that merely boots as an example).
This allows me to replace the build system (as previously mentioned) and structure the
project as I see fit (which will probably be very similar anyway).

You can see my current plans on the
[projects page](https://github.com/Sparkpin/spinel/projects/1).

## Building Spinel

To build Spinel, you will need a cross compiler of some sort.
[Here](https://wiki.osdev.org/GCC_Cross-Compiler) is a nice article on that. Currently,
Spinel supports the i[3-7]86-elf targets. Eventually, it will get its own compiler
target.

Whenever you're ready, just run build.sh. It requires the aforementioned cross compiler,
some variation of Make (GNU Make will work), and dialog. You can choose to build an ISO
and launch QEMU from the build script. This will require QEMU, grub-mkrescue (probably
via a GRUB package), and likely xorriso.

## Using Spinel

Spinel is not in a usable state at the moment. However, I hope to get to user mode as
soon as I can. Realistically, Spinel will never be in a state that it could serve as
one's daily driver operating system... but I can dream. :P
