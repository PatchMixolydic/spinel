PROGNAME=libhakurei

CFLAGS ?= -O2 -g
CPPFLAGS ?=
LDFLAGS ?=
LIBS ?=

DESTDIR ?=
PREFIX ?= /usr/local
EXEC_PREFIX ?= $(PREFIX)
BOOTDIR ?= $(EXEC_PREFIX)/boot
INCLUDEDIR ?= $(PREFIX)/include

CFLAGS := $(CFLAGS) -Wall -Wextra -Wpedantic
CPPFLAGS := $(CPPFLAGS) -Iinclude
LIBS := $(LIBS) -lc
KERN_LIBS := $(KERN_LIBS) -lckern

OBJS=\
ansi.o\
terminal.o\

KERN_OBJS=$(OBJS:.o=.kern.o)

BINARIES = libhakurei.a libhakukern.a

.PHONY: all clean test install install-libs
.SUFFIXES: .o .kern.o .c

all: $(BINARIES)

libhakurei.a: $(OBJS)
	$(AR) rcs $@ $(OBJS)

libhakukern.a: $(KERN_OBJS)
	$(AR) rcs $@ $(KERN_OBJS)

.c.o:
	$(CC) -MD -c $< -o $@ -std=c11 $(CFLAGS) $(CPPFLAGS)

.c.kern.o:
	$(CC) -MD -c $< -o $@ -std=c11 $(CFLAGS) $(CPPFLAGS) $(KERN_LIBS)

clean:
	rm -f $(BINARIES) *.a
	rm -f $(OBJS) $(KERN_OBJS) *.o */*.o */*/*.o
	rm -f $(OBJS:.o=.d) $(KERN_OBJS:.o=.d) *.d */*.d */*/*.d

test:
	echo "$(PROGNAME) has no unit tests."

install: install-headers install-libs

install-headers:
	mkdir -p $(DESTDIR)$(INCLUDEDIR)
	cp -R --preserve=timestamps include/. $(DESTDIR)$(INCLUDEDIR)/.

install-libs: $(BINARIES)
	mkdir -p $(DESTDIR)$(LIBDIR)
	cp $(BINARIES) $(DESTDIR)$(LIBDIR)

-include $(OBJS:.o=.d)
-include $(KERN_OBJS:.o=.d)
