#!/bin/sh
SYSTEM_HEADER_PROJECTS="\
  src/lib/libc \
  src/kernel \
  src/lib/libdastr \
  src/lib/libhakurei \
  src/spinunit \
  src/thirdparty/miscInc\
"

PROJECTS="\
  src/spinunit \
  src/lib/libc \
  src/lib/libdastr \
  src/lib/libhakurei \
  src/kernel \
"

export MAKE="${MAKE:-make} -j`expr $(nproc) + 1`"
export HOST=${HOST:-`./defaultHost.sh`}

export TESTAR=ar
export TESTNASM=nasm
# set test C compiler to user's if they have a preference, else gcc
TESTCC=$CC
[ -z "$TESTCC" ] && TESTCC=gcc
export TESTCC

export AR=${HOST}-ar
export NASM=nasm
export CC=${HOST}-gcc

export PREFIX=/usr
export EXEC_PREFIX=$PREFIX
export BOOTDIR=/boot
export LIBDIR=$EXEC_PREFIX/lib
export INCLUDEDIR=$PREFIX/include

export ROOTDIR=`pwd`

export CFLAGS='-O2 -g -fno-omit-frame-pointer -fstack-protector-all \
  -D__Commit__=\"$(shell git rev-parse --short HEAD)\" \
  -D__DateTime__="\"$(shell date +"%F %I:%M%P %Z")\""'
export CPPFLAGS=''
export LDFLAGS='-g'

# Configure the cross-compiler to use the desired system root.
export SYSROOT="`pwd`/sysroot"
export CC="$CC --sysroot=$SYSROOT"

# Work around that the -elf gcc targets doesn't have a system include directory
# because it was configured with --without-headers rather than --with-sysroot.
if echo "$HOST" | grep -Eq -- '-elf($|-)'; then
  export CC="$CC -isystem=$INCLUDEDIR"
fi
