#!/usr/bin/env python3
"""
The build system for Spinel. It is meant to be executed on its own,
although it might be possible to use its component parts for other means.
Options can be preconfigured through buildConfig.py or they can be
given on the command line at build time. Command line arguments take
precedence.

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

import argparse, os, shlex, shutil, subprocess, sys
import buildConfig

TripletTemplate = "{}-elf"
DefaultCrossCompilerTemplate = "{triplet}-gcc"
ValidActions = ["build", "clean"]

triplet = None
crossCompilerTemplate = DefaultCrossCompilerTemplate

def printErr(*args, **kwargs):
    """Convenience function to print to stderr"""
    print(*args, file = sys.stderr, **kwargs)

def getCrossCompiler(target = None):
    """Get cross compiler executable for a given target"""
    return crossCompilerTemplate.format(
        triplet =
            triplet if triplet is not None else TripletTemplate.format(target)
    )

def crossCompilerExists(target):
    """Check if cross compiler is installed for a target"""
    return shutil.which(getCrossCompiler(target)) is not None

def tryRun(args):
    """Try to run a program, and raise an exception if it fails"""
    res = subprocess.run(args)
    if res.returncode != 0:
        raise RuntimeError(
            f"Process {args} failed with return code {res.returncode}"
        )

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
    os.chdir(origDir)

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
        "-x", "--compTemplate",
        help = "Cross compiler template. Useful if you're not using GCC. "
            "The default template is '{}'."
            .format(DefaultCrossCompilerTemplate)
    )
    parser.add_argument(
        "-c", "--configureArgs", default = [], type = shlex.split,
        help = "Arguments for newlib's configure script. Please note that "
            "--target is provided automatically."
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

    # Needs to be done before validating args to check if cross compiler exists
    if args.compTemplate is not None:
        crossCompilerTemplate = args.compTemplate

    if not areArgsValid(args):
        parser.print_help()
        sys.exit(1)

    triplet = TripletTemplate.format(args.target)

    print(f"Target {args.target} (triplet {triplet})")
    print(f"{os.cpu_count()} CPUs")
    print(f"{args.action.title()}ing")

    if args.action == "build":
        print("Building newlib")
        buildConfigure(
            "newlib-cygwin",
            ["--target", triplet] + args.configureArgs
        )
    elif args.action == "clean":
        clean("newlib-cygwin")

    print("Done")
