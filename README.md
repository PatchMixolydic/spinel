# Spinel

[![Build Status](https://travis-ci.com/Sparkpin/spinel.svg?branch=master)](https://travis-ci.com/Sparkpin/spinel)

*"I'm so excited to meet you!"*

Spinel is an operating system. It was written to allow me to explore the world
of systems programming after trying and failing to wait patiently for it to be
taught to me in university.

Right now, Spinel is in a very prototypical state. It currently only supports
x86. Future plans include x86_64 support, user mode, multitasking, and someday,
a GUI. Other architectures might be supported as well (Motorola 68k, PowerPC,
ARM, and RISC-V  are all likely candidates).

Spinel was originally started in December 2019. That version of Spinel is still
available on the [original](https://github.com/Sparkpin/spinel/tree/original)
branch. In February 2020, I decided to restart development on Spinel to try and
strive for a cleaner code base and a better memory manager. The reset reached
feature parity at the end of that same month (2020-02-27) and was merged that
day.

You can see my current plans on the
[projects page](https://github.com/Sparkpin/spinel/projects/1).

## Building Spinel

To build Spinel, you will need a cross compiler of some sort.
[Here](https://wiki.osdev.org/GCC_Cross-Compiler) is a nice article on that.
Currently, Spinel supports the i[3-7]86-elf targets. Eventually, it will get
its own compiler target.

Whenever you're ready, just run build.sh. It requires the aforementioned cross
compiler, some variation of Make, and dialog. You can choose to build an ISO
and launch QEMU from the build script. This will require QEMU, grub-mkrescue
(probably via a GRUB package), and likely xorriso.

Eventually, I'd like for there to be live images for download, along with the
ability to build an ISO or virtual machine disk image without using GRUB.

## Using Spinel

Spinel is not in a usable state at the moment. However, I hope to get to user mode as
soon as I can. Realistically, Spinel will never be in a state that it could serve as
one's daily driver operating system... but I can dream. :P
