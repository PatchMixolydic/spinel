DEFAULT_HOST != ../../defaultHost.sh
HOST ?= DEFAULT_HOST
HOSTARCH != ../../tripletToArch.sh $(HOST)

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
CPPFLAGS := $(CPPFLAGS) -Iinclude
LIBCKERN_CFLAGS := $(CFLAGS)
LIBCKERN_CPPFLAGS := $(CPPFLAGS) -D__LIBCKERN

ARCHDIR = arch/$(HOSTARCH)

include $(ARCHDIR)/make.config

CFLAGS := $(CFLAGS) $(LIBC_ARCH_CFLAGS)
CPPFLAGS := $(CPPFLAGS) $(LIBC_ARCH_CPPFLAGS)
LIBCKERN_CFLAGS := $(LIBCKERN_CFLAGS) $(LIBC_ARCH_CFLAGS)
LIBCKERN_CPPFLAGS := $(LIBCKERN_CPPFLAGS) $(LIBC_ARCH_CPPFLAGS)

FREESTANDING_OBJS = \
$(LIBC_ARCH_FREESTANDING_OBJS) \
ctype/isdigit.o \
stdio/printf.o \

HOSTED_OBJS = \
$(LIBC_ARCH_HOSTED_OBJS) \

OBJS=\
$(FREESTANDING_OBJS) \
$(HOSTED_OBJS) \

LIBCKERN_OBJS=$(FREESTANDING_OBJS:.o=.libckern.o)

BINARIES = libckern.a # libc.a

.PHONY: all clean install install-libs
.SUFFIXES: .o .libckern.o .c .asm

all: $(BINARIES)

libc.a: $(OBJS)
	$(AR) rcs $@ $(OBJS)

libckern.a: $(LIBCKERN_OBJS)
	$(AR) rcs $@ $(LIBCKERN_OBJS)

.c.o:
	$(CC) -MD -c $< -o $@ -std=c11 $(CFLAGS) $(CPPFLAGS)

.asm.o:
	$(NASM) -f elf32 -o $@ $<

.c.libckern.o:
	$(CC) -MD -c $< -o $@ -std=c11 $(LIBCKERN_CFLAGS) $(LIBCKERN_CPPFLAGS)

.asm.libckern.o:
	$(NASM) -f elf32 -o $@ $<

clean:
	rm -f $(BINARIES) *.a
	rm -f $(OBJS) $(LIBCKERN_OBJS) *.o */*.o */*/*.o
	rm -f $(OBJS:.o=.d) $(LIBCKERN_OBJS:.o=.d) *.d */*.d */*/*.d

install: install-headers install-libs

install-headers:
	mkdir -p $(DESTDIR)$(INCLUDEDIR)
	cp -R --preserve=timestamps include/. $(DESTDIR)$(INCLUDEDIR)/.

install-libs: $(BINARIES)
	mkdir -p $(DESTDIR)$(LIBDIR)
	cp $(BINARIES) $(DESTDIR)$(LIBDIR)

-include $(OBJS:.o=.d)
-include $(LIBCKERN_OBJS:.o=.d)
