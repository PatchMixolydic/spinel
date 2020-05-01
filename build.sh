#!/bin/sh

set -e

. ./config.sh

ONLYTHIS=false

help() {
    printf "Usage: build.sh [-o] [-h] [action]\n"
    printf "\t-o - Only perform this action, do not perform prerequisites\n"
    printf "\t-h - Print help\n"
    printf "\taction - An action to perform\n"
    printf "\t\tclean - Clean up build files\n"
    printf "\t\theaders - Install header files\n"
    printf "\t\tbuild - Build Spinel\n"
    printf "\t\ttests - Run unit tests\n"
    printf "\t\tiso - Generate an ISO file\n"
    printf "\t\tqemu - Run Spinel in QEMU\n"
    printf "\t\tdisassemble - Disassemble the kernel into a diffable format\n"
    printf "\t\tProviding no action will open an interactive menu\n"
}

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
    if [ $ONLYTHIS = false ]; then
        clean
        headers
    fi
    mkdir -p "$SYSROOT"
    for PROJECT in $PROJECTS; do
        (cd $PROJECT && DESTDIR="$SYSROOT" $MAKE install)
    done
}

tests() {
    if [ $ONLYTHIS = false ]; then
        build
    fi

    OLDAR="$AR"
    OLDNASM="$NASM"
    OLDCC="$CC"

    export AR="$TESTAR"
    export NASM="$TESTNASM"
    export CC="$TESTCC"

    for PROJECT in $PROJECTS; do
        (cd $PROJECT && $MAKE test)
    done

    export AR="$OLDAR"
    export NASM="$OLDNASM"
    export CC="$OLDCC"
}

iso() {
    if [ $ONLYTHIS = false ]; then
        build
    fi
    TMPDIR=$(mktemp -d -t spinelTmp_ 2>/dev/null) || TMPDIR=/tmp/spineliso$$ \
        && mkdir $TMPDIR
    trap "rm -rf $TMPDIR" 1 5 15 EXIT
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
    if [ $ONLYTHIS = false ]; then
        iso
    fi
    qemu-system-$(./tripletToArch.sh $HOST) -cdrom build/spinel.iso -s -m 512 \
        -soundhw pcspk -enable-kvm -serial stdio -device ich9-ahci
}

disassemble() {
    build
    objdump -d --no-show-raw-insn \
        --disassembler-options=intel-mnemonic src/kernel/spinel.elf | \
        sed -E 's/^[A-Fa-f0-9]{8}:\t//g' > spinel.asm
    echo "Disassembled into spinel.asm"
}

rotate() {
    echo $1 | tr 'A-Za-z' 'N-ZA-Mn-za-m'
}

while getopts 'qoh' OPTION; do
  case "$OPTION" in
    o)
      ONLYTHIS=1
      ;;

    h)
      help
      exit 0
      ;;
    ?)
      help >&2
      exit 1
      ;;
  esac
done
shift "$(($OPTIND -1))"

rotated=$(rotate $1)

if [ "$1" = "clean" ] || [ "$1" = "headers" ] || [ "$1" = "build" ] || \
[ "$1" = "iso" ] || [ "$1" = "qemu" ] || [ "$1" = "tests" ] || \
[ "$1" = "disassemble" ]; then
    $1
# Are there Easter eggs in this program?
elif [ $(rotate $1) = "zbb" ]; then
    echo $(rotate "\\g\\gGuvf ohvyq fpevcg qbrf abg unir Fhcre Pbj Cbjref.")
elif [ $(rotate $1) = "png" ]; then
    echo $(rotate "\\g\\gGuvf ohvyq fpevcg unf Fhcre Png Cbjref.")
elif [ $(rotate $1) = "zrbj" ]; then
    echo $(rotate "Lbh'er trggvat pybfre.")
elif [ $(rotate $1) = "aln" ] || [ $(rotate $1) = "alnn" ] || \
    [ $(rotate $1) = "alnnn" ] || [ $(rotate $1) = "alna" ];
then
    echo $(rotate "Fcvary pbclevtug Ehol Ynmhyv 2019-2020")
    echo $(rotate "Gunax lbh sbe lbhe vagrerfg!")
    echo $(rotate "V ubcr gur pbqr vfa'g gbb onq :C")
    echo $(rotate "\\g\\gLbh abj unir Fhcre Png Cbjref.")
else
    TMPFILE=$(tempfile 2>/dev/null) || TMPFILE=/tmp/spinelmenu$$
    trap "rm -f $TMPFILE" 1 5 15 EXIT

    dialog --clear --menu \
    "Select an option\n\
    Unless you specify the -o option, selecting an action may\n\
    perform its prerequisites (ex. qemu will run iso, which\n\
    runs build, which runs headers and clean).\n\
    (Note: you can also specify these on the command line)" \
    0 0 0 \
    "help" "Help for using this script" \
    "clean" "Delete all build files" \
    "headers" "Copy headers to sysroot" \
    "build" "Perform a full build" \
    "tests" "Run unit tests" \
    "iso" "Create a bootable ISO" \
    "qemu" "Boot Spinel in QEMU" \
    "disassemble" "Dissamble the kernel into a diffable format" \
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
