# Spinel

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
* It gives me reason to replace the build system (I'm experimenting with Python right
now)
* I can use a third party libc instead of writing my own, at least to get started (I'm
using newlib right now)
* Spinel is no longer based on OSDev's "Meaty Skeleton" tutorial (which covers good
practices for storing files, using a simple kernel that merely boots as an example).
This allows me to replace the build system (as previously mentioned) and structure the
project as I see fit (which will probably be very similar anyway).

