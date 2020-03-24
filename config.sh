#!/bin/sh
SYSTEM_HEADER_PROJECTS="\
  src/libc \
  src/kernel \
  src/libdastr \
  src/libhakurei \
  src/thirdparty/miscInc\
"
PROJECTS="\
  src/libc \
  src/libdastr \
  src/libhakurei \
  src/kernel \
"

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

export ROOTDIR=`pwd`

export CFLAGS='-O2 -g -D__Commit__=\"$(shell git rev-parse --short HEAD)\"\
  -D__DateTime__="\"$(shell date +"%F %I:%M%P %Z")\""'
export CPPFLAGS=''

# Configure the cross-compiler to use the desired system root.
export SYSROOT="`pwd`/sysroot"
export CC="$CC --sysroot=$SYSROOT"

# Work around that the -elf gcc targets doesn't have a system include directory
# because it was configured with --without-headers rather than --with-sysroot.
if echo "$HOST" | grep -Eq -- '-elf($|-)'; then
  export CC="$CC -isystem=$INCLUDEDIR"
fi
