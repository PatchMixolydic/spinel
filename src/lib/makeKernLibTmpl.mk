# Template to make a library for both kernelspace and userspace
ifndef PROGNAME
$(error PROGNAME wasn't provided)
endif
ifndef OBJS
$(error OBJS wasn't provided for $(PROGNAME))
endif

CFLAGS ?= -O2 -g -Wall -Wpedantic -Wextra
KERN_CFLAGS ?= $(CFLAGS)
KERN_CFLAGS := -ffreestanding $(KERN_CFLAGS)
KERN_CPPFLAGS ?= $(CPPFLAGS)
KERN_CPPFLAGS := $(KERN_CPPFLAGS) -D__KERNEL
KERN_LIBS ?= -lckern
KERN_OBJS ?= $(OBJS:.o=.kern.o)
BINARIES ?= $(PROGNAME).a $(PROGNAME)kern.a

include $(ROOTDIR)/src/lib/makeLibTmpl.mk

$(PROGNAME)kern.a: $(KERN_OBJS)
	$(AR) rcs $@ $(KERN_OBJS)

%.kern.o: %.c
	$(CC) -MD -c $< -o $@ -std=c11 $(KERN_CFLAGS) $(KERN_CPPFLAGS) $(KERN_LIBS)

clean::
	rm -f $(KERN_OBJS) *.o */*.o */*/*.o
	rm -f $(KERN_OBJS:.o=.d) *.d */*.d */*/*.d

-include $(KERN_OBJS:.o=.d)
