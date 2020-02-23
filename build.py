#!/usr/bin/env python3.8
"""
The build system for Spinel. It is meant to be executed on its own,
although it might be possible to use its component parts for other means.
Options can be preconfigured through buildConfig.py or they can be
given on the command line at build time. Command line arguments take
precedence.

This is a mess!!! I will hopefully clean it up later™. Right now I am just
trying to get it working without making too much of a mess

When thinking of how to manage Spinel's build system, two obvious options
lay before me: CMake and Autoconf. Naturally, I selected Python, for a
multitude of reasons. Primarily, I already know how Python works, and I
know that Python is very flexible. For this personal project, I felt that
my best option would be to roll my own build system for efficiency reasons.
I will try my best to not make the interface too exotic.

While there are some disadvantages to writing this in Python (namely the
loss of config.sub), I hope this build system might be simple enough for my
purposes.
"""

import argparse, fnmatch, glob, os, shlex, shutil, subprocess, sys, tempfile
import buildConfig

TripletTemplate = "{}-elf"
DefaultCrossCompilerTemplate = "{triplet}-gcc"
ValidActions = ["build", "clean", "qemu"]

def setEnvVar(key, val):
    if sys.platform.startswith("freebsd") or sys.platform.startswith("darwin"):
        """
        Python 3 docs:
        Note
        On some platforms, including FreeBSD and Mac OS X, setting environ may
        cause memory leaks. Refer to the system documentation for putenv().
        https://docs.python.org/3/library/os.html?highlight=environ#os.environ
        """
        os.putenv(key, val)
    else:
        # And yet, the very same docs claim os.environ is preferred
        os.environ[key] = val

def setEnvVarIfUndefined(key, val):
    if os.getenv(key) is None:
        setEnvVar(key, val)

cFlags = os.getenv("CFLAGS", "-g -O2")
cFlags += " -Wall -Wextra -Wpedantic -std=c11 -ffreestanding"
triplet = os.getenv("triplet")
crossCompilerTemplate = os.getenv(
    "crossCompilerTemplate", DefaultCrossCompilerTemplate
)

def printErr(*args, **kwargs):
    """Convenience function to print to stderr"""
    print(*args, file = sys.stderr, **kwargs)

def getCrossCompiler():
    """Get cross compiler executable for a given target"""
    return crossCompilerTemplate.format(triplet = triplet)

def crossCompilerExists(target):
    """Check if cross compiler is installed for a target"""
    return shutil.which(getCrossCompiler()) is not None

def getCanonicalArch(arch = None):
    arch = os.getenv('target') if arch is None else arch
    if fnmatch.fnmatch(arch, "i[3-7]86"):
        # x86 is wack
        return "i386"
    return arch

def tryRun(args):
    """Try to run a program, and raise an exception if it fails"""
    res = subprocess.run(args)
    if res.returncode != 0:
        raise RuntimeError(
            f"Process {args} failed with return code {res.returncode}"
        )

def compile(filename):
    """
    Tries to compile a file into an object file, guessing what it is by extension
    """
    splitName = os.path.split(filename)[1].split(".")
    objectName = ".".join(splitName[:-1]) + ".o"

    if splitName[-1] == "c":
        tryRun([
            getCrossCompiler(),
            "-c", filename,
            "-o", objectName
        ] + shlex.split(os.getenv("cFlags")))
    elif splitName[-1] == "asm":
        tryRun(
            ["nasm", "-o", objectName] + shlex.split(os.getenv("nasmFlags")) +
            [filename]
        )
    else:
        printErr(f"Ignoring file with unknown extension {filename}")

def buildConfigure(rootDir, configureArgs = [], makeArgs = []):
    """
    Builds a project in rootDir with the standard ./configure && make sequence
    """
    origDir = os.getcwd()
    os.chdir(rootDir)
    os.makedirs("build", exist_ok = True)
    os.chdir("build")
    tryRun(["../configure"] + configureArgs)
    tryRun(["make", f"-j{os.cpu_count() + 1}"] + makeArgs)
    setEnvVar("DESTDIR", os.getenv("SYSROOT"))
    tryRun(["make", "install"])
    os.chdir(origDir)

def buildSpinel():
    """
    Builds Spinel
    """
    origDir = os.getcwd()
    sysrootDir = os.path.join(os.getcwd(), os.getenv("SYSROOT"))
    includeDir = os.path.join(sysrootDir, "usr/include")
    os.makedirs(includeDir, exist_ok = True)
    shutil.copytree("include", includeDir, dirs_exist_ok = True)
    os.makedirs("build", exist_ok = True)
    os.chdir("build")

    for root, dirs, files in os.walk(".."):
        if "thirdparty" in dirs:
            dirs.remove("thirdparty")
        for f in files:
            if f.split(".")[-1] in ["asm", "c"]:
                compile(os.path.join(root, f))

    crtbegin = subprocess.run([
        getCrossCompiler(), "-print-file-name=crtbegin.o"
    ], stdout=subprocess.PIPE).stdout.decode('utf-8').strip()
    crtend = subprocess.run([
        getCrossCompiler(), "-print-file-name=crtend.o"
    ], stdout=subprocess.PIPE).stdout.decode('utf-8').strip()

    objFiles = [
        "crti.o",
        crtbegin
    ]
    objFiles += [x for x in glob.glob("*.o") if not x in ["crti.o", "crtn.o"]]
    objFiles += [
        crtend,
        "crtn.o"
    ]

    tryRun(
        [getCrossCompiler(), "-T",
        f"../src/kernel/arch/{getCanonicalArch()}/linker.ld",
        "-o", "spinel.elf", "-ffreestanding", "-O2", "-nostdlib", "-lgcc"] +
        objFiles
    )

    os.makedirs(os.path.join(sysrootDir, "boot"), exist_ok = True)
    shutil.copy("spinel.elf", os.path.join(sysrootDir, "boot", "spinel.elf"))
    os.chdir(origDir)

def makeISO():
    with tempfile.TemporaryDirectory(prefix = "spinelISO_") as tempDir:
        shutil.copytree("sysroot", tempDir, dirs_exist_ok = True)
        grubDir = os.path.join(tempDir, "boot/grub")
        os.makedirs(grubDir, exist_ok = True)
        with open(os.path.join(grubDir, "grub.cfg"), "w") as f:
            f.write('menuentry "spinel" {')
            f.write('\tmultiboot /boot/spinel.elf')
            f.write('}')
        tryRun([
            "grub-mkrescue", "-o", os.path.join("build", "spinel.iso"), tempDir
        ])

def runQEmu():
    tryRun([
        f"qemu-system-{getCanonicalArch()}",
        "-cdrom", "build/spinel.iso", "-s", "-m", "512", "-soundhw", "pcspk"
    ])

def clean(rootDir):
    """Clean a project in rootDir by deleting its build material."""
    origDir = os.getcwd()
    os.chdir(rootDir)
    shutil.rmtree("build", ignore_errors = True)
    os.chdir(origDir)

if __name__ == "__main__":
    def areArgsValid(args):
        """
        Due to buildConfig, we can't just mark args as required. This is
        a workaround for that.
        """
        if args.action not in ValidActions:
            printErr(f"Unknown action {args.action}")
            return False
        elif args.target is None:
            printErr("Invalid configuration")
            return False
        elif not crossCompilerExists(args.target):
            printErr(f"Couldn't find cross compiler for {args.target}")
            return False

        return True

    parser = argparse.ArgumentParser(
        description = "Builds Spinel. Configuration is pulled from "
            "buildConfig.py. Command line arguments take precedence."
    )
    parser.add_argument(
        "-t", "--target",
        help = "Target architecture for Spinel. It is assumed "
            "that the target triplet is {}. Required."
            .format(TripletTemplate.format("TARGET"))
    )
    parser.add_argument(
        "-x", "--compTemplate", default = DefaultCrossCompilerTemplate,
        help = "Cross compiler template. Useful if you're not using GCC. "
            "The default template is '{}'."
            .format(DefaultCrossCompilerTemplate)
    )
    parser.add_argument(
        "-c", "--configureArgs", default = [], type = shlex.split,
        help = "Arguments for newlib's configure script. Please note that "
            "--target and --prefix are provided automatically."
    )
    parser.add_argument(
        "-s", "--sysroot",
        default = os.getenv("SYSROOT", os.path.join(os.getcwd(), "sysroot")),
        help = "Location for the sysroot. Defaults to the value of the "
            "$SYSROOT environment variable, or if that isn't set, ./sysroot."
    )
    parser.add_argument(
        "-f", "--cFlags", default = os.getenv("CFLAGS", "-g -O2"),
        help = "Flags for the C compiler. Defaults to the $CFLAGS environment "
            "variable, or '-g -O2' if that doesn't exist. Warnings, standard "
            "library, and freestanding mode will be set for you no matter "
            "what."
    )
    parser.add_argument(
        "-n", "--nasmFlags", default = os.getenv("NASMFLAGS", "-felf32"),
        help = "Flags for NASM. Defaults to the $NASMFLAGS environment "
            "variable, or '-felf32' if that doesn't exist."
    )
    parser.add_argument(
        "action", default = "build", nargs='?',
        help = "Action to perform. Defaults to 'build'. "
            f"Valid actions are: {str(ValidActions)[1:-1]}"
    )
    args = parser.parse_args()

    # Gets all values from buildConfig that aren't private
    # (esp. values like __builtins__)
    config = {
        k: v for k, v in vars(buildConfig).items() if not k.startswith("_")
    }
    # Create a dict with values from both args and config.
    # In the event of a conflict, args takes precedence.
    for k, v in config.items():
        if k not in vars(args) or vars(args).get(k) is None:
            vars(args).update({k: v})
    # And now, we can set these all to be environment variables...
    for k, v in vars(args).items():
        if v is None:
            v = ""
        elif type(v) == list:
            v = " ".join(v)
        setEnvVar(k, v)

    setEnvVarIfUndefined("triplet", TripletTemplate.format(args.target))
    setEnvVar("SYSROOT", args.sysroot)
    triplet = os.getenv("triplet")
    cFlags += " -Wall -Wextra -Wpedantic -std=c11 -ffreestanding"

    if not areArgsValid(args):
        parser.print_help()
        sys.exit(1)

    print(f"Target {args.target} (triplet {triplet})")
    print(f"{os.cpu_count()} CPUs")
    print(f"{args.action.title()}ing")

    if args.action == "build":
        print("Building newlib")
        buildConfigure(
            "thirdparty/newlib-cygwin",
            ["--target", triplet, "--prefix", "/usr"] + args.configureArgs
        )
        print("Building Spinel")
        buildSpinel()
        print("Making ISO")
        makeISO()
    elif args.action == "qemu":
        runQEmu()
    elif args.action == "clean":
        clean("thirdparty/newlib-cygwin")
        clean(".")

    print("Done")
