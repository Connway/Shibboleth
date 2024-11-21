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
  'include/mimalloc/atomic.h',
  'include/mimalloc/internal.h',
  'include/mimalloc/prim.h',
  'include/mimalloc/track.h',
  'include/mimalloc/types.h',
  'include/mimalloc-new-delete.h',
  'include/mimalloc-override.h',
  'include/mimalloc.h',
  'src/bitmap.h',
  'src/prim/windows/etw.h',
]

source_files = [
  'src/alloc-aligned.c',
  'src/alloc-posix.c',
  'src/alloc.c',
  'src/arena.c',
  'src/bitmap.c',
  'src/heap.c',
  'src/init.c',
  'src/libc.c',
  'src/options.c',
  'src/os.c',
  'src/page.c',
  'src/prim/prim.c',
  'src/random.c',
  'src/segment-map.c',
  'src/segment.c',
  'src/stats.c',
]

extra_files = [
]

include_dirs = [
  'include',
]

build_args = [
]

msvc_args = []

if is_debug
  build_args += ['-DMI_DEBUG=3']
endif

if is_msvc
  msvc_args += ['-D_CRT_SECURE_NO_WARNINGS']
endif

mimalloc = static_library(
  'mimalloc',
  header_files,
  source_files,
  include_directories: include_dirs,
  c_args: default_c_args + build_args + msvc_args,
  override_options: {
    'werror': false,
  }
)

mimalloc_dep = declare_dependency(
  include_directories: include_dirs,
  compile_args: build_args,
  link_with: [mimalloc],
)

all_deps += mimalloc
