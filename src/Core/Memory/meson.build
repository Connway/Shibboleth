header_files = [
  'include/Shibboleth_Allocator.h',
  'include/Shibboleth_IAllocator.h',
  'include/Shibboleth_Memory.h',
]

source_files = [
  'Shibboleth_Allocator.cpp',
  'Shibboleth_Memory.cpp',
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

memory = shared_library(
  'Memory',
  header_files,
  source_files,
  include_directories: include_dirs,
  cpp_args: default_cpp_args + build_args,
  dependencies: default_dependencies + dependencies,
)

memory_dep = declare_dependency(
  include_directories: ['include'],
  link_with: [memory],
)

all_core += memory

