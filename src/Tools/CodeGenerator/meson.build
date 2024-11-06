header_files = [
  'include/CodeGen_ReflectionHeaderGenerator.h',
  'include/CodeGen_Utils.h',
]

source_files = [
  'CodeGen_ReflectionHeaderGenerator.cpp',
  'CodeGen_Utils.cpp',
  'CodeGenerator.cpp',
]

extra_files = [
]

include_dirs = [
  'include',
]

build_args = [
]

dependencies = [
  argparse_dep,
  fmt_no_link_dep,

  gaff_dep,
]

code_generator = executable(
  'CodeGenerator',
  header_files,
  source_files,
  include_directories: include_dirs,
  extra_files: extra_files,
  cpp_args: default_cpp_args + build_args,
  dependencies: dependencies,
)

all_tools += code_generator
