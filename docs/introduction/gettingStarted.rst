Getting Started
===============

Spinel is designed for ease of development. Although Spinel is (as of the time
of writing) a hobbyist operating system, it is also intended to be rather
comfortable to use, at least for the power user.

Building Spinel
---------------

To build Spinel, you will need a cross compiler of some sort.
An article on how to accomplish this can be found
`here <https://wiki.osdev.org/GCC_Cross-Compiler>`_.
Currently, Spinel supports the i[3-7]86-elf targets. Eventually, it will get
its own compiler target.

Whenever you're ready, just run build.sh. It requires the aforementioned cross
compiler, some variation of Make, and dialog. You can choose to build an ISO
and launch QEMU from the build script. This will require QEMU, grub-mkrescue
(probably via a GRUB package), and likely xorriso.

Eventually, the build system will be able to create a disk image on its own,
obviating the need for grub-mkrescue at the very least.

Using Spinel
------------

Currently, Spinel is not in a usable state. It is under heavy development, and
there is currently no user interface to speak of. This will be addressed in the
near future.
