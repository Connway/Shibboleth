header_files = [
  'include/Shibboleth_Allocator.h',
  'include/Shibboleth_IAllocator.h',
  'include/Shibboleth_Memory.h',
]

source_files = [
  'src/Shibboleth_Allocator.cpp',
  'src/Shibboleth_Memory.cpp',
]

extra_files = [
]

include_dirs = [
  'include',
  '../Engine/include',
]

build_args = [
  '-DIS_MEMORY',
  '-DCHECK_FOR_LEAKS',
]

static_build_args = [
  '-DMEMORY_STATIC',
]

dependencies = [
  mimalloc_dep,

  gaff_dep,
]

if is_debug
  build_args += [
    '-DCHECK_FOR_DOUBLE_FREE',
    '-DCHECK_FOR_MISALIGNED_POINTER'
  ]
endif

if is_windows
  dependencies += compiler.find_library('Dbghelp')
endif

memory_static = static_library(
  'MemoryStatic',
  header_files,
  source_files,
  include_directories: include_dirs,
  cpp_args: default_cpp_args + build_args + static_build_args,
  dependencies: default_static_dependencies + dependencies,
  install: true,
  install_dir: 'bin',
)

memory = shared_library(
  'Memory',
  header_files,
  source_files,
  include_directories: include_dirs,
  cpp_args: default_cpp_args + build_args,
  dependencies: default_dependencies + dependencies,
  install: true,
  install_dir: 'bin',
)

memory_static_dep = declare_dependency(
  include_directories: ['include'],
  compile_args: static_build_args,
  link_with: [memory_static],
)

memory_dep = declare_dependency(
  include_directories: ['include'],
  link_with: [memory],
)

all_core += memory

