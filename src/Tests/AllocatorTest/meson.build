header_files = [
]

source_files = [
  'AllocatorTest.cpp',
  '../Shibboleth_TestMain.cpp',
]

generated_files = [
]

extra_files = [
]

include_dirs = [
]

build_args = [
]

dependencies = [
  catch2_dep,

  engine_static_dep,
  memory_static_dep,
]

if is_windows
  dependencies += compiler.find_library('Dbghelp')
elif is_linux
  dependencies += dependency('dl')
endif

allocator_test = executable(
  'AllocatorTest',
  header_files,
  source_files,
  include_directories: include_dirs,
  extra_files: extra_files,
  cpp_args: default_cpp_args + build_args,
  dependencies: dependencies,
)

test('Allocator', allocator_test, is_parallel: true)

all_tests += [allocator_test]
