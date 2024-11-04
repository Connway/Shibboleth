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
)

libpng_dep = declare_dependency(
  include_directories: ['.'],
  compile_args: build_args,
  link_with: [libpng],
)

all_deps += libpng
