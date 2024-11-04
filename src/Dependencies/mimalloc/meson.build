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
