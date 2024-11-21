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
  'png.h',
  'pngconf.h',
  'pngdebug.h',
  'pnginfo.h',
  'pnglibconf.h',
  'pngpriv.h',
  'pngstruct.h',
]

source_files = [
  'png.c',
  'pngerror.c',
  'pngget.c',
  'pngmem.c',
  'pngpread.c',
  'pngread.c',
  'pngrio.c',
  'pngrtran.c',
  'pngrutil.c',
  'pngset.c',
  'pngtest.c',
  'pngtrans.c',
  'pngwio.c',
  'pngwrite.c',
  'pngwtran.c',
  'pngwutil.c',
]

extra_files = [
]

include_dirs = [
  '../../Dependencies/zlib-ng',
]

build_args = [
]

msvc_args = []

if is_debug
  build_args += ['-DPNG_DEBUG']
endif

if is_msvc
  msvc_args += ['-D_CRT_SECURE_NO_WARNINGS']
endif

libpng = static_library(
  'libpng',
  header_files,
  source_files,
  include_directories: include_dirs,
  c_args: default_c_args + build_args + msvc_args,
  override_options: {
    'werror': false,
  },
)

libpng_dep = declare_dependency(
  include_directories: ['.'],
  compile_args: build_args,
  link_with: [libpng],
)

all_deps += libpng
