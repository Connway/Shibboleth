header_files = [
]

source_files = [
  'JSONToMessagePack.cpp',
]

extra_files = [
]

include_dirs = [
]

build_args = [
]

dependencies = [
  gaff_dep,
]

json_to_message_pack = executable(
  'JSONToMessagePack',
  header_files,
  source_files,
  include_directories: include_dirs,
  extra_files: extra_files,
  cpp_args: default_cpp_args + build_args,
  dependencies: dependencies,
  install: true,
  install_dir: 'Tools',
)

all_tools += json_to_message_pack
