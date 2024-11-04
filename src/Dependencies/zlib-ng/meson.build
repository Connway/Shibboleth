header_files = [
  'adler32_p.h',
  'arch/arm/acle_intrins.h',
  'arch/arm/arm_features.h',
  'arch/arm/arm_functions.h',
  'arch/arm/neon_intrins.h',
  'arch/generic/chunk_permute_table.h',
  'arch/generic/generic_functions.h',
  'arch/power/crc32_constants.h',
  'arch/power/fallback_builtins.h',
  'arch/power/power_features.h',
  'arch/power/power_functions.h',
  'arch/power/slide_ppc_tpl.h',
  'arch/riscv/riscv_features.h',
  'arch/riscv/riscv_functions.h',
  'arch/s390/dfltcc_common.h',
  'arch/s390/dfltcc_deflate.h',
  'arch/s390/dfltcc_detail.h',
  'arch/s390/dfltcc_inflate.h',
  'arch/s390/s390_features.h',
  'arch/s390/s390_functions.h',
  'arch/x86/adler32_avx2_p.h',
  'arch/x86/adler32_avx512_p.h',
  'arch/x86/adler32_ssse3_p.h',
  'arch/x86/crc32_fold_pclmulqdq_tpl.h',
  'arch/x86/crc32_fold_vpclmulqdq_tpl.h',
  'arch/x86/crc32_pclmulqdq_tpl.h',
  'arch/x86/x86_features.h',
  'arch/x86/x86_functions.h',
  'arch/x86/x86_intrins.h',
  'arch_functions.h',
  'chunkset_tpl.h',
  'compare256_rle.h',
  'cpu_features.h',
  'crc32.h',
  'crc32_braid_comb_p.h',
  'crc32_braid_p.h',
  'crc32_braid_tbl.h',
  'deflate.h',
  'deflate_p.h',
  'fallback_builtins.h',
  'functable.h',
  'gzguts.h',
  'inffast_tpl.h',
  'inffixed_tbl.h',
  'inflate.h',
  'inflate_p.h',
  'inftrees.h',
  'insert_string_tpl.h',
  'match_tpl.h',
  'trees.h',
  'trees_emit.h',
  'trees_tbl.h',
  'zbuild.h',
  'zconf-ng.h',
  'zconf.h',
  'zendian.h',
  'zlib-ng.h',
  'zlib.h',
  'zlib_name_mangling.h',
  'zutil.h',
  'zutil_p.h',
]

source_files = [
  'adler32.c',
  'arch/generic/adler32_c.c',
  'arch/generic/adler32_fold_c.c',
  'arch/generic/chunkset_c.c',
  'arch/generic/compare256_c.c',
  'arch/generic/crc32_braid_c.c',
  'arch/generic/crc32_fold_c.c',
  'arch/generic/slide_hash_c.c',
  'compress.c',
  'cpu_features.c',
  'crc32.c',
  'crc32_braid_comb.c',
  'deflate.c',
  'deflate_fast.c',
  'deflate_huff.c',
  'deflate_medium.c',
  'deflate_quick.c',
  'deflate_rle.c',
  'deflate_slow.c',
  'deflate_stored.c',
  'functable.c',
  'gzlib.c',
  'gzread.c',
  'gzwrite.c',
  'infback.c',
  'inflate.c',
  'inftrees.c',
  'insert_string.c',
  'insert_string_roll.c',
  'trees.c',
  'uncompr.c',
  'zutil.c',
]

extra_files = [
]

include_dirs = [
]

build_args = [
  '-DWITH_GZFILEOP',
]

if is_x64
  build_args += ['-DX86_FEATURES']

  source_files += [
    'arch/x86/adler32_avx2.c',
    'arch/x86/adler32_avx512.c',
    'arch/x86/adler32_avx512_vnni.c',
    'arch/x86/adler32_sse42.c',
    'arch/x86/adler32_ssse3.c',
    'arch/x86/chunkset_avx2.c',
    'arch/x86/chunkset_sse2.c',
    'arch/x86/chunkset_ssse3.c',
    'arch/x86/compare256_avx2.c',
    'arch/x86/compare256_sse2.c',
    'arch/x86/crc32_pclmulqdq.c',
    'arch/x86/crc32_vpclmulqdq.c',
    'arch/x86/slide_hash_avx2.c',
    'arch/x86/slide_hash_sse2.c',
    'arch/x86/x86_features.c',
  ]

elif is_arm64
  build_args += ['-DARM_FEATURES']

  source_files = [
    'arch/arm/adler32_neon.c',
    'arch/arm/arm_features.c',
    'arch/arm/chunkset_neon.c',
    'arch/arm/compare256_neon.c',
    'arch/arm/crc32_acle.c',
    'arch/arm/slide_hash_armv6.c',
    'arch/arm/slide_hash_neon.c',
  ]
endif

msvc_args = []

if is_msvc
  msvc_args += ['-D_CRT_SECURE_NO_WARNINGS']
  build_args += ['-D_LARGEFILE64_SOURCE', '-D_LFS64_LARGEFILE']
endif

if is_windows
elif is_linux
  build_args += ['-DHAVE_ATTRIBUTE_ALIGNED']

elif is_mac
  build_args += ['-DHAVE_ATTRIBUTE_ALIGNED']
endif

zlib_ng = static_library(
  'zlib-ng',
  header_files,
  source_files,
  include_directories: include_dirs,
  c_args: default_c_args + build_args + msvc_args,
  override_options: {
    'werror': false,
  }
)

zlib_ng_dep = declare_dependency(
  include_directories: ['.'],
  compile_args: build_args,
  link_with: zlib_ng,
)

all_deps += zlib_ng
