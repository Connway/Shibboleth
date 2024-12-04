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

import argparse
import pathlib
import shutil
import os

parser = argparse.ArgumentParser(prog="shibboleth_post_install", description="Does some small file cleanup when running install command.")

parser.add_argument(
    "-r", "--root",
    type=pathlib.Path,
    help="The folder to run the post install cleanup on.",
)

args = parser.parse_args()

library_extensions = [".a", "lib"]

root = pathlib.Path(".")

if args.root:
    root = args.root

shutil.rmtree(root / "lib")

for ext in library_extensions:
    library_files = sorted(root.glob("**/*" + ext))

    for lib_file in library_files:
        os.remove(lib_file)
