PROGNAME = spinunit

CFLAGS ?= -O2 -g

DESTDIR ?=
PREFIX ?= /usr/local
INCLUDEDIR ?= $(PREFIX)/include

CFLAGS := $(CFLAGS) -Wall -Wextra -Wpedantic -Iinclude

.PHONY: all cleam test install install-headers

all: install

clean:

install: install-headers

install-headers:
	mkdir -p $(DESTDIR)$(INCLUDEDIR)
	cp -R --preserve=timestamps include/. $(DESTDIR)$(INCLUDEDIR)/.

test:
	$(CC) test.c -o test$(PROGNAME) $(CFLAGS)
	./test$(PROGNAME)
	rm test$(PROGNAME)
