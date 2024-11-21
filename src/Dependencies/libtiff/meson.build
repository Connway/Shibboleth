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
  'libtiff/t4.h',
  'libtiff/tif_config.h',
  'libtiff/tif_dir.h',
  'libtiff/tif_fax3.h',
  'libtiff/tif_hash_set.h',
  'libtiff/tif_predict.h',
  'libtiff/tiff.h',
  'libtiff/tiffconf.h',
  'libtiff/tiffio.h',
  'libtiff/tiffiop.h',
  'libtiff/tiffvers.h',
  'libtiff/uvcode.h',
  'port/libport.h',
  'port/libport_config.h',
]

source_files = [
  'libtiff/mkg3states.c',
  'libtiff/tif_aux.c',
  'libtiff/tif_close.c',
  'libtiff/tif_codec.c',
  'libtiff/tif_color.c',
  'libtiff/tif_compress.c',
  'libtiff/tif_dir.c',
  'libtiff/tif_dirinfo.c',
  'libtiff/tif_dirread.c',
  'libtiff/tif_dirwrite.c',
  'libtiff/tif_dumpmode.c',
  'libtiff/tif_error.c',
  'libtiff/tif_extension.c',
  'libtiff/tif_fax3.c',
  'libtiff/tif_fax3sm.c',
  'libtiff/tif_flush.c',
  'libtiff/tif_getimage.c',
  'libtiff/tif_hash_set.c',
  'libtiff/tif_jbig.c',
  'libtiff/tif_jpeg.c',
  'libtiff/tif_jpeg_12.c',
  'libtiff/tif_lerc.c',
  'libtiff/tif_luv.c',
  'libtiff/tif_lzma.c',
  'libtiff/tif_lzw.c',
  'libtiff/tif_next.c',
  'libtiff/tif_ojpeg.c',
  'libtiff/tif_open.c',
  'libtiff/tif_packbits.c',
  'libtiff/tif_pixarlog.c',
  'libtiff/tif_predict.c',
  'libtiff/tif_print.c',
  'libtiff/tif_read.c',
  'libtiff/tif_strip.c',
  'libtiff/tif_swab.c',
  'libtiff/tif_thunder.c',
  'libtiff/tif_tile.c',
  'libtiff/tif_version.c',
  'libtiff/tif_warning.c',
  'libtiff/tif_webp.c',
  'libtiff/tif_write.c',
  'libtiff/tif_zip.c',
  'libtiff/tif_zstd.c',
  'port/dummy.c',
  'port/getopt.c',
  'libtiff/tif_stream.cxx',
]

extra_files = [
]

include_dirs = [
  'port',
  '../../Dependencies/zlib-ng',
  '../../Dependencies/zstd',
]

build_args = [
]

if is_windows
  source_files += ['libtiff/tif_win32.c']
else
  source_files += ['libtiff/tif_unix.c']
endif

if is_msvc
  build_args += ['-D_CRT_SECURE_NO_WARNINGS']
endif

libtiff = static_library(
  'libtiff',
  header_files,
  source_files,
  include_directories: include_dirs,
  c_args: default_c_args + build_args,
  cpp_args: default_cpp_args + build_args,
  override_options: {
    'werror': false,
  }
)

libtiff_dep = declare_dependency(
  include_directories: ['.', 'libtiff'],
  link_with: [libtiff],
)

all_deps += libtiff
