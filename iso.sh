#!/bin/sh
set -e
. ./build.sh

mkdir -p isodir
mkdir -p isodir/boot
mkdir -p isodir/boot/grub

cp sysroot/boot/nyaaOS.kernel isodir/boot/nyaaOS.kernel
cat > isodir/boot/grub/grub.cfg << EOF
menuentry "nyaaOS" {
	multiboot /boot/nyaaOS.kernel
}
EOF
grub-mkrescue -o nyaaOS.iso isodir
