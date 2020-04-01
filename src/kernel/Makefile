DEFAULT_HOST != $(ROOTDIR)/defaultHost.sh
HOST ?= DEFAULT_HOST
HOSTARCH != $(ROOTDIR)/tripletToArch.sh $(HOST)

CFLAGS ?= -O2 -g
CPPFLAGS ?=
LDFLAGS ?=
LIBS ?=

DESTDIR ?=
PREFIX ?= /usr/local
EXEC_PREFIX ?= $(PREFIX)
BOOTDIR ?= $(EXEC_PREFIX)/boot
INCLUDEDIR ?= $(PREFIX)/include

CFLAGS := $(CFLAGS) -ffreestanding -Wall -Wextra -Wpedantic
CPPFLAGS := $(CPPFLAGS) -D__Kernel -Iinclude
LDFLAGS := $(LDFLAGS)
LIBS := $(LIBS) -nostdlib -lgcc -lhakukern -ldastr -lckern

ARCHDIR = arch/$(HOSTARCH)

include $(ARCHDIR)/make.config

CFLAGS:=$(CFLAGS) $(KERNEL_ARCH_CFLAGS)
CPPFLAGS:=$(CPPFLAGS) $(KERNEL_ARCH_CPPFLAGS)
LDFLAGS:=$(LDFLAGS) $(KERNEL_ARCH_LDFLAGS)
LIBS:=$(LIBS) $(KERNEL_ARCH_LIBS)

KERNEL_OBJS=\
$(KERNEL_ARCH_OBJS) \
concurrency/mutex.o\
core/kernelInfo.o \
core/kernelMain.o \
core/panic.o\
filesystem/vfs.o\
filesystem/zero.o\
input/keymap.o\
memory/alloc.o\

OBJS=\
$(ARCHDIR)/core/crti.o \
$(ARCHDIR)/core/crtbegin.o \
$(KERNEL_OBJS) \
$(ARCHDIR)/core/crtend.o \
$(ARCHDIR)/core/crtn.o \

LINK_LIST=\
$(LDFLAGS) \
$(ARCHDIR)/core/crti.o \
$(ARCHDIR)/core/crtbegin.o \
$(KERNEL_OBJS) \
$(LIBS) \
$(ARCHDIR)/core/crtend.o \
$(ARCHDIR)/core/crtn.o \

.PHONY: all clean install install-headers install-kernel test
.SUFFIXES: .o .c .asm

all: spinel.elf

spinel.elf: $(OBJS) $(ARCHDIR)/linker.ld
	$(CC) -T $(ARCHDIR)/linker.ld -o $@ $(CFLAGS) $(LINK_LIST)
	grub-file --is-x86-multiboot2 spinel.elf

$(ARCHDIR)/core/crtbegin.o $(ARCHDIR)/core/crtend.o:
	OBJ=`$(CC) $(CFLAGS) $(LDFLAGS) -print-file-name=$(@F)` && cp "$$OBJ" $@

.c.o:
	$(CC) -MD -c $< -o $@ -std=c11 $(CFLAGS) $(CPPFLAGS)

.asm.o:
	$(NASM) -f $(KERNEL_ARCH_EXEC_FORMAT) -o $@ $<

clean:
	rm -f spinel.elf
	rm -f $(OBJS) *.o */*.o */*/*.o
	rm -f $(OBJS:.o=.d) *.d */*.d */*/*.d

test:
	echo "The kernel currently does not have any unit tests."

install: install-headers install-kernel

install-headers:
	mkdir -p $(DESTDIR)$(INCLUDEDIR)
	cp -R --preserve=timestamps include/. $(DESTDIR)$(INCLUDEDIR)/.

install-kernel: spinel.elf
	mkdir -p $(DESTDIR)$(BOOTDIR)
	cp spinel.elf $(DESTDIR)$(BOOTDIR)

-include $(OBJS:.o=.d)
