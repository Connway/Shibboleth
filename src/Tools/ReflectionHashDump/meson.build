header_files = [
]

source_files = [
  'ReflectionHashDump.cpp',
]

extra_files = [
]

include_dirs = [
]

build_args = [
]

dependencies = [
  gleam_dep,
  engine_dep,
  memory_dep,
]

if is_windows
  dependencies += compiler.find_library('Dbghelp')
elif is_linux
  dependencies += [dependency('dl'), dependency('pthread')]
endif

reflection_hash_dump = executable(
  'ReflectionHashDump',
  header_files,
  source_files,
  include_directories: include_dirs,
  extra_files: extra_files,
  cpp_args: default_cpp_args + build_args,
  dependencies: dependencies,
)

all_tools += reflection_hash_dump
