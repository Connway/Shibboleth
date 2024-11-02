header_files = [
  'catch_amalgamated.hpp',
]

source_files = [
  'catch_amalgamated.cpp',
]

include_dirs = [
  '../../Dependencies/EASTL/include',
  '../../Dependencies/rapidjson',
  '../../Frameworks/Gaff/include',
  '../../Core/Engine/include',
  '../../Core/Memory/include',
]

build_args = [
  '-DCATCH_AMALGAMATED_CUSTOM_MAIN'
]

catch2 = static_library(
  'Catch2',
  header_files,
  source_files,
  include_directories: include_dirs,
  cpp_args: default_cpp_args + build_args,
  # pic: true,
)

