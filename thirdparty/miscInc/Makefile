DEFAULT_HOST != ../../defaultHost.sh
HOST ?= DEFAULT_HOST
HOSTARCH != ../../tripletToArch.sh $(HOST)

DESTDIR ?=
PREFIX ?= /usr/local
EXEC_PREFIX ?= $(PREFIX)
INCLUDEDIR ?= $(PREFIX)/include

.PHONY: all clean install install-headers

all: spinel.elf

clean:

install: install-headers

install-headers:
	mkdir -p $(DESTDIR)$(INCLUDEDIR)
	cp -R --preserve=timestamps include/. $(DESTDIR)$(INCLUDEDIR)/.
