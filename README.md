# Spinel

[![Build Status](https://travis-ci.com/Sparkpin/spinel.svg?branch=spinel-nd)](https://travis-ci.com/Sparkpin/spinel)

*"...and [she] told me that **I~~I~/I can make a change!**"*

Spinel is a simple operating system written to allow me to explore the
wonderful world of systems programming. Currently, it only supports
amd64, but more platforms are planned to be supported in the future,
including i386.

You can see my current plans on the
[projects page](https://github.com/Sparkpin/spinel/projects/1).

## History of Spinel

Spinel was originally started in December 2019. That version of Spinel is still
available on the [original](https://github.com/Sparkpin/spinel/tree/original)
branch. In February 2020, I decided to restart development on Spinel to try and
strive for a cleaner code base and a better memory manager. The reset reached
feature parity at the end of that same month (2020-02-27) and was merged that
day.

Eventually, I started to feel that C was holding back my ability to write
an expressive operating system. I started thinking about porting the
project to C++. Finally, in June 2020, I took the dive and rewrote Spinel
for the second time, this time in Rust.

## Why Rust?

In short: Cargo's pretty handy, the freestanding environment is incredible, the
language itself is pretty nice, and the safety guarantees are a nice bonus. :>

## Building Spinel

To build Spinel, you will need [Cargo](https://www.rust-lang.org/tools/install),
the Rust package manager.

After getting set up, install the nightly build of Rust's toolchain, which
contains several useful features that Spinel makes use of:
```sh
$ rustup toolchain install nightly
```

After that, you'll need a few helper utilities (feel free to collapse these
into one `cargo install` command):
```sh
$ cargo install cargo-xbuild # For cross-compiling Rust's freestanding environment
$ cargo install llvm-tools-preview bootimage # To create the bootable disk image
```

Then, you can use `cargo xbuild` to build the disk image. If you have QEMU
installed, you can use `cargo xrun` to build the disk image and launch QEMU.

You can also use `cargo xdoc` to generate the documentation, and
`cargo xdoc --open` to view it.

## Using Spinel

Spinel is not in a usable state at the moment. However, I hope to get to user mode as
soon as I can. Realistically, Spinel may never be in a state that it could serve as
one's daily driver operating system... but I can dream. :P

If you just want to run Spinel, please see Building Spinel.

## Contributing

Right now, Spinel is in a very early state, so it is probably not too
interesting to contribute to right now. However, if you are interested on
getting on board once user mode applications are possible, please note that
to keep Spinel from turning into Flamewar Central, there is a basic
[code of conduct](https://github.com/Sparkpin/spinel/blob/master/CodeOfConduct.md)
for contributors. These rules should be fairly obvious, but feel free to have
a look at them.

Additionally, contributors will likely be interested in licensing information,
which is available in the following section.

## License

Spinel is licensed under the
[MIT license](https://github.com/Sparkpin/spinel/blob/master/LICENSE).
This includes every file that comprises Spinel, unless it is overriden
by a license included within the file or within a closer parent directory.
In other words, to find the license for the file, look for a license in the
file, then in its parent directory, then in that directory's parent directory,
and so on until you hit the project root.

Contributors who do not wish to cede their copyright to the
project's copyright holder (Ruby Lazuli Lord) may include licenses in files
they have written. These licenses must be compatible with Spinel's MIT license.
Any contributor who does not add a license will, as stated in the GitHub terms
of service, implicitly cede their copyright to the project's copyright holder.

If someone wrote a signficiant amount of code, you write a significant amount of
code alongside it (or replacing it) in the same file, and you wish to apply
your own license to the file or you wish to add your name to a license already
in the file, please consult everyone who edited the file previously.
