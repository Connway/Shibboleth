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
  'mz.h',
  'mz_compat.h',
  'mz_crypt.h',
  'mz_os.h',
  'mz_strm.h',
  'mz_strm_buf.h',
  'mz_strm_bzip.h',
  'mz_strm_libcomp.h',
  'mz_strm_lzma.h',
  'mz_strm_mem.h',
  'mz_strm_os.h',
  'mz_strm_pkcrypt.h',
  'mz_strm_split.h',
  'mz_strm_wzaes.h',
  'mz_strm_zlib.h',
  'mz_strm_zstd.h',
  'mz_zip.h',
  'mz_zip_rw.h',
  'unzip.h',
  'zip.h',
]

source_files = [
  'minigzip.c',
  'minizip.c',
  'mz_compat.c',
  'mz_crypt.c',
  'mz_os.c',
  'mz_strm.c',
  'mz_strm_buf.c',
  'mz_strm_mem.c',
  'mz_strm_pkcrypt.c',
  'mz_strm_split.c',
  'mz_strm_wzaes.c',
  'mz_strm_zlib.c',
  'mz_strm_zstd.c',
  'mz_zip.c',
  'mz_zip_rw.c',
]

extra_files = [
]

include_dirs = [
  '../../Dependencies/zlib-ng',
  '../../Dependencies/zstd',
]

build_args = [
  '-DHAVE_ZLIB',
  '-DMZ_ZIP_NO_MAIN',
]

if is_windows
  source_files += [
    'mz_os_win32.c',
    'mz_strm_os_win32.c',
  ]
elif is_linux
  source_files += [
    'mz_os_posix.c',
    'mz_strm_os_posix.c',
  ]
elif is_mac
  source_files += [
    'mz_os_posix.c',
    'mz_strm_os_posix.c',
    'mz_crypt_apple.c',
  ]
endif

minizip_ng = static_library(
  'minizip-ng',
  header_files,
  source_files,
  include_directories: include_dirs,
  c_args: default_c_args + build_args,
  override_options: {
    'werror': false,
  }
)

minizip_ng_dep = declare_dependency(
  include_directories: ['.'],
  link_with: [minizip_ng],
)

all_deps += minizip_ng
