# Template to make a library for userspace
ifndef PROGNAME
$(error PROGNAME wasn't provided)
endif
ifndef OBJS
$(error OBJS wasn't provided for $(PROGNAME))
endif

CFLAGS ?= -O2 -g -Wall -Wextra -Wpedantic
CPPFLAGS ?= -Iinclude
LDFLAGS ?=
LIBS ?= -lc

DESTDIR ?=
PREFIX ?= /usr/local
EXEC_PREFIX ?= $(PREFIX)
BOOTDIR ?= $(EXEC_PREFIX)/boot
INCLUDEDIR ?= $(PREFIX)/include

BINARIES ?= $(PROGNAME).a

.PHONY:: all clean test install install-libs

all: $(BINARIES)

$(PROGNAME).a: $(OBJS)
	$(AR) rcs $@ $(OBJS)

%.o: %.c
	$(CC) -MD -c $< -o $@ -std=c11 $(CFLAGS) $(CPPFLAGS) $(LIBS)

clean::
	rm -f $(BINARIES) *.a
	rm -f $(OBJS) *.o */*.o */*/*.o
	rm -f $(OBJS:.o=.d) *.d */*.d */*/*.d

test:
	# TODO
	echo "$(PROGNAME) has no unit tests."

install: install-headers install-libs

install-headers:
	mkdir -p $(DESTDIR)$(INCLUDEDIR)
	cp -R --preserve=timestamps include/. $(DESTDIR)$(INCLUDEDIR)/.

install-libs: $(BINARIES)
	mkdir -p $(DESTDIR)$(LIBDIR)
	cp $(BINARIES) $(DESTDIR)$(LIBDIR)

-include $(OBJS:.o=.d)
