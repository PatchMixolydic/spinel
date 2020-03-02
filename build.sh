#!/bin/sh

set -e

. ./config.sh

clean() {
    for PROJECT in $PROJECTS; do
        (cd $PROJECT && $MAKE clean)
    done

    rm -rf sysroot
    rm -rf build
}

headers() {
    mkdir -p "$SYSROOT"
    ORIGDIR=`pwd`
    for PROJECT in $SYSTEM_HEADER_PROJECTS; do
        (cd $PROJECT && DESTDIR="$SYSROOT" $MAKE install-headers; cd $ORIGDIR)
    done
}

build() {
    headers
    mkdir -p "$SYSROOT"
    for PROJECT in $PROJECTS; do
        (cd $PROJECT && DESTDIR="$SYSROOT" $MAKE install)
    done
}

iso() {
    build
    TMPDIR=$(mktemp -d -t spinelTmp_ 2>/dev/null) || TMPDIR=/tmp/spineliso$$ \
        && mkdir $TMPDIR
    trap "rm -rf $TMPDIR" 1 5 15
    mkdir -p $TMPDIR/boot/grub
    cp -R sysroot/* $TMPDIR
    cat > $TMPDIR/boot/grub/grub.cfg << EOF
menuentry "spinel" {
    multiboot2 /boot/spinel.elf
}
EOF
    mkdir -p build
    grub-mkrescue -o build/spinel.iso $TMPDIR
    rm -rf $TMPDIR
}

qemu() {
    iso
    qemu-system-$(./tripletToArch.sh $HOST) -cdrom build/spinel.iso -s -m 512 \
        -soundhw pcspk -enable-kvm
}

if [ "$1" = "clean" ] || [ "$1" = "headers" ] || [ "$1" = "build" ] || \
[ "$1" = "iso" ] || [ "$1" = "qemu" ]; then
    $1
else
    TMPFILE=$(tempfile 2>/dev/null) || TMPFILE=/tmp/spinelmenu$$
    trap "rm -f $TMPFILE" 1 5 15

    dialog --clear --menu \
    "Select an option\n\
    (Note: you can also specify these on the command line)" \
    0 0 0 "clean" "Delete all build files" \
    "headers" "Copy headers to sysroot" \
    "build" "Perform a full build" \
    "iso" "Create a bootable ISO (first building Spinel)" \
    "qemu" "Boot Spinel in QEMU (first building Spinel)" \
    2> $TMPFILE

    RETCODE=$?
    CHOICE=`cat $TMPFILE`
    case $RETCODE in
        0)
            $CHOICE
        ;;

        *);;
    esac
fi
