#####################################################################################
# Copyright (C) by Nicholas LaCroix
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
# THE SOFTWARE.
#####################################################################################

import subprocess
import argparse
import sys
import os

parser = argparse.ArgumentParser(prog="shibboleth_build", description="Abstracts the execution of build commands.")

parser.add_argument(
    "configuration",
    type=str,
    default="debug",
    choices=["debug", "debugoptimized", "profile", "release", "Debug", "DebugOptimized", "Profile", "Release"],
    help="Which configuration to generate and build."
)

parser.add_argument("-i", "--install", action="store_true", help="Installs built binaries into destinatino directory.")
parser.add_argument("-rb", "--rebuild", action="store_true", help="Performs a clean and build.")
parser.add_argument("-c", "--clean", action="store_true", help="Deletes the build directory.")
parser.add_argument("-t", "--target", type=str, help="Build a specific target.")

args = parser.parse_args()

configuration = args.configuration.lower()
build_dir = "build/" + configuration

install_cmd = ["meson", "install", "-C", "build/" + configuration, "--destdir", "../../workingdir"]
setup_cmd = ["meson", "setup", build_dir, "--vsenv"]
build_cmd = ["meson", "compile", "-C", build_dir]
clean_cmd = ["rm", "-rf", build_dir]

if args.target:
    build_cmd = build_cmd + [target]

if sys.platform == "win32":
    clean_cmd = ["rmdir", build_dir.replace("/", "\\"), "/S", "/Q"]

if args.clean or args.rebuild:
    if os.path.isdir(build_dir):
        print("Deleting build directory: " + build_dir)

        if subprocess.call(clean_cmd, shell=True):
            exit()

    if args.clean and not args.rebuild:
        exit()

if not os.path.exists(build_dir):
    if subprocess.call(setup_cmd, shell=True):
        exit()

if subprocess.call(build_cmd, shell=True):
    exit()

if args.install:
    subprocess.call(install_cmd, shell=True)
