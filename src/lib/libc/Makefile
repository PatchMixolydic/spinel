PROGNAME=libc

DEFAULT_HOST != $(ROOTDIR)/defaultHost.sh
HOST ?= DEFAULT_HOST
HOSTARCH != $(ROOTDIR)/tripletToArch.sh $(HOST)

CFLAGS ?= -O2 -g -Wall -Wpedantic -Wextra -ffreestanding
CPPFLAGS ?=
LDFLAGS ?=
LIBS ?=

ARCHDIR = arch/$(HOSTARCH)
GENERICARCHDIR = arch/generic

include $(ARCHDIR)/make.config

CFLAGS := $(CFLAGS) $(LIBC_ARCH_CFLAGS)
CPPFLAGS := $(CPPFLAGS) $(LIBC_ARCH_CPPFLAGS)
KERN_CFLAGS := $(KERN_CFLAGS) $(LIBC_ARCH_CFLAGS)
KERN_CPPFLAGS := $(KERN_CPPFLAGS) $(LIBC_ARCH_CPPFLAGS)
KERN_LIBS :=

FREESTANDING_OBJS = \
$(LIBC_ARCH_FREESTANDING_OBJS) \
ctype/isalpha.o \
ctype/isdigit.o \
errno.o \
stackprotect.o\
stdio/perror.o \
stdio/printf.o \
stdio/vprintf.o \
stdlib/calloc.o \
stdlib/malloc.o \
stdlib/itoa.o \
string/memcmp.o \
string/memcpy.o \
string/memmove.o \
string/memset.o \
string/strcpy.o \
string/strcspn.o \
string/strdup.o \
string/strerror.o \
string/strlcpy.o \
string/strlen.o \
string/strncmp.o \
string/strncpy.o \
string/strpbrk.o \
string/strchr.o \
string/strrchr.o \
string/strspn.o \
string/strtok_r.o \
time/daysinmonth.o \
time/daysinyear.o \
time/isleapyear.o \
time/time.o \

HOSTED_OBJS = \
$(LIBC_ARCH_HOSTED_OBJS) \

OBJS=\
$(FREESTANDING_OBJS) \
$(HOSTED_OBJS) \

KERN_OBJS=$(FREESTANDING_OBJS:.o=.kern.o)

BINARIES = $(PROGNAME)kern.a # $(PROGNAME).a

%.o: %.asm
	$(NASM) -f elf32 -o $@ $<

%.kern.o: %.asm
	$(NASM) -f elf32 -o $@ $<

include $(ROOTDIR)/src/lib/makeKernLibTmpl.mk
