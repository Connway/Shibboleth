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
  'include/fmt/args.h',
  'include/fmt/chrono.h',
  'include/fmt/color.h',
  'include/fmt/compile.h',
  'include/fmt/core.h',
  'include/fmt/format-inl.h',
  'include/fmt/format.h',
  'include/fmt/os.h',
  'include/fmt/ostream.h',
  'include/fmt/printf.h',
  'include/fmt/ranges.h',
  'include/fmt/std.h',
  'include/fmt/xchar.h',
]

source_files = [
  # 'src/fmt.cc',
  'src/format.cc',
  'src/os.cc',
]

extra_files = [
]

include_dirs = [
  'include',
]

build_args = [
]

fmt = static_library(
  'fmt',
  header_files,
  source_files,
  include_directories: include_dirs,
  c_args: default_c_args + build_args,
)

fmt_no_link_dep = declare_dependency(
  include_directories: include_dirs,
  compile_args: ['-DFMT_HEADER_ONLY']
)

fmt_dep = declare_dependency(
  dependencies: fmt_no_link_dep,
  link_with: [fmt],
)

all_deps += fmt
