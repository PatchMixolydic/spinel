#!/bin/sh
SYSTEM_HEADER_PROJECTS="src thirdparty/miscInc"
PROJECTS="src"

export MAKE="${MAKE:-make} -j`expr $(nproc) + 1`"
export HOST=${HOST:-`./defaultHost.sh`}

export AR=${HOST}-ar
export NASM=nasm
export CC=${HOST}-gcc

export PREFIX=/usr
export EXEC_PREFIX=$PREFIX
export BOOTDIR=/boot
export LIBDIR=$EXEC_PREFIX/lib
export INCLUDEDIR=$PREFIX/include

export CFLAGS='-O2 -g -DDEBUG'
export CPPFLAGS=''

# Configure the cross-compiler to use the desired system root.
export SYSROOT="`pwd`/sysroot"
export CC="$CC --sysroot=$SYSROOT"

# Work around that the -elf gcc targets doesn't have a system include directory
# because it was configured with --without-headers rather than --with-sysroot.
if echo "$HOST" | grep -Eq -- '-elf($|-)'; then
  export CC="$CC -isystem=$INCLUDEDIR"
fi
