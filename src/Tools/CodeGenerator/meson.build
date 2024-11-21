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
  'include/CodeGen_ReflectionHeaderGenerator.h',
  'include/CodeGen_Utils.h',
]

source_files = [
  'CodeGen_ReflectionHeaderGenerator.cpp',
  'CodeGen_Utils.cpp',
  'CodeGenerator.cpp',
]

extra_files = [
]

include_dirs = [
  'include',
]

build_args = [
]

dependencies = [
  argparse_dep,
  fmt_no_link_dep,

  gaff_dep,
]

code_generator = executable(
  'CodeGenerator',
  header_files,
  source_files,
  include_directories: include_dirs,
  extra_files: extra_files,
  cpp_args: default_cpp_args + build_args,
  dependencies: dependencies,
)

all_tools += code_generator
