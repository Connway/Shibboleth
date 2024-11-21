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
  'common/allocations.h',
  'common/bits.h',
  'common/bitstream.h',
  'common/compiler.h',
  'common/cpu.h',
  'common/debug.h',
  'common/error_private.h',
  'common/fse.h',
  'common/huf.h',
  'common/mem.h',
  'common/pool.h',
  'common/portability_macros.h',
  'common/threading.h',
  'common/xxhash.h',
  'common/zstd_deps.h',
  'common/zstd_internal.h',
  'common/zstd_trace.h',
  'compress/clevels.h',
  'compress/hist.h',
  'compress/zstd_compress_internal.h',
  'compress/zstd_compress_literals.h',
  'compress/zstd_compress_sequences.h',
  'compress/zstd_compress_superblock.h',
  'compress/zstd_cwksp.h',
  'compress/zstd_double_fast.h',
  'compress/zstd_fast.h',
  'compress/zstd_lazy.h',
  'compress/zstd_ldm.h',
  'compress/zstd_ldm_geartab.h',
  'compress/zstd_opt.h',
  'compress/zstdmt_compress.h',
  'decompress/zstd_ddict.h',
  'decompress/zstd_decompress_block.h',
  'decompress/zstd_decompress_internal.h',
  'dictBuilder/cover.h',
  'dictBuilder/divsufsort.h',
  'zdict.h',
  'zstd.h',
  'zstd_errors.h',
]

source_files = [
  'common/debug.c',
  'common/entropy_common.c',
  'common/error_private.c',
  'common/fse_decompress.c',
  'common/pool.c',
  'common/threading.c',
  'common/xxhash.c',
  'common/zstd_common.c',
  'compress/fse_compress.c',
  'compress/hist.c',
  'compress/huf_compress.c',
  'compress/zstd_compress.c',
  'compress/zstd_compress_literals.c',
  'compress/zstd_compress_sequences.c',
  'compress/zstd_compress_superblock.c',
  'compress/zstd_double_fast.c',
  'compress/zstd_fast.c',
  'compress/zstd_lazy.c',
  'compress/zstd_ldm.c',
  'compress/zstd_opt.c',
  'compress/zstdmt_compress.c',
  'decompress/huf_decompress.c',
  'decompress/zstd_ddict.c',
  'decompress/zstd_decompress.c',
  'decompress/zstd_decompress_block.c',
  'dictBuilder/cover.c',
  'dictBuilder/divsufsort.c',
  'dictBuilder/fastcover.c',
  'dictBuilder/zdict.c',
]

extra_files = [
]

include_dirs = [
]

build_args = [
  '-DZSTD_MULTITHREAD=1',
]

if is_x64
  if not is_windows
    source_files += 'decompress/huf_decompress_amd64.S'
  endif
endif

zstd = static_library(
  'zstd',
  header_files,
  source_files,
  include_directories: include_dirs,
  cpp_args: default_cpp_args + build_args,
)

zstd_dep = declare_dependency(
  include_directories: ['.'],
  compile_args: build_args,
  link_with: [zstd],
)

all_deps += zstd
