header_files = [
  'mpack-common.h',
  'mpack-config.h',
  'mpack-defaults.h',
  'mpack-expect.h',
  'mpack-node.h',
  'mpack-platform.h',
  'mpack-reader.h',
  'mpack-writer.h',
  'mpack.h',
]

source_files = [
  'mpack-common.c',
  'mpack-expect.c',
  'mpack-node.c',
  'mpack-platform.c',
  'mpack-reader.c',
  'mpack-writer.c',
]

extra_files = [
]

include_dirs = [
]

build_args = [
  '-DMPACK_HAS_CONFIG'
]

mpack = static_library(
  'mpack',
  header_files,
  source_files,
  include_directories: include_dirs,
  c_args: default_c_args + build_args,
)

mpack_dep = declare_dependency(
  include_directories: ['.'],
  compile_args: build_args,
  link_with: [mpack],
)

all_deps += mpack
