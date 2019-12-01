#!/bin/sh
set -e
. ./build.sh

mkdir -p isodir
mkdir -p isodir/boot
mkdir -p isodir/boot/grub

cp sysroot/boot/spinel.kernel isodir/boot/spinel.kernel
cat > isodir/boot/grub/grub.cfg << EOF
menuentry "spinel" {
	multiboot /boot/spinel.kernel
}
EOF
grub-mkrescue -o spinel.iso isodir
