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

header_files = [
  'mpack-common.h',
  'mpack-config.h',
  'mpack-defaults.h',
  'mpack-expect.h',
  'mpack-node.h',
  'mpack-platform.h',
  'mpack-reader.h',
  'mpack-writer.h',
  'mpack.h',
]

source_files = [
  'mpack-common.c',
  'mpack-expect.c',
  'mpack-node.c',
  'mpack-platform.c',
  'mpack-reader.c',
  'mpack-writer.c',
]

extra_files = [
]

include_dirs = [
]

build_args = [
  '-DMPACK_HAS_CONFIG'
]

mpack = static_library(
  'mpack',
  header_files,
  source_files,
  include_directories: include_dirs,
  c_args: default_c_args + build_args,
)

mpack_dep = declare_dependency(
  include_directories: ['.'],
  compile_args: build_args,
  link_with: [mpack],
)

all_deps += mpack
