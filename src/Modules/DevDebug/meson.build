header_files = [
  'include/Shibboleth_DebugAttributes.h',
  'include/Shibboleth_DebugManager.h',
  'include/Shibboleth_DebugSystem.h',
  'include/Shibboleth_IDebugManager.h',
]

source_files = [
  'src/Shibboleth_DebugAttributes.cpp',
  'src/Shibboleth_DebugManager.cpp',
  'src/Shibboleth_DebugSystem.cpp',
  'src/Shibboleth_DevDebugModule.cpp',
]

module_source_files = [
  'src/Shibboleth_DevDebugModule.cpp',
]

generated_files = [
  custom_target(
    'Gen_ReflectionInit.h',
    output: 'Gen_ReflectionInit.h',
    command: [code_generator, 'module_header', '-rp', '@SOURCE_ROOT@', '-of', '@OUTPUT@', '-m', 'DevDebug'],
    depends: [code_generator],
    build_always_stale: true,
  ),
]

extra_files = [
]

include_dirs = [
  'include',
]

build_args = [
  '-DSHIB_STATIC',
]

module_build_args = [
]

dependencies = [
  engine_dep,
  memory_dep,
]

dev_debug = static_library(
  'DevDebug',
  header_files,
  source_files,
  generated_files,
  include_directories: include_dirs,
  cpp_args: default_cpp_args + build_args,
  dependencies: dependencies,
)

dev_debug_dep = declare_dependency(
  include_directories: include_dirs,
  dependencies: dependencies,
  link_with: [dev_debug],
  sources: [generated_files],
)

dev_debug_module = shared_library(
  'DevDebugModule',
  module_source_files,
  cpp_args: default_cpp_args + module_build_args,
  dependencies: dev_debug_dep,
)

all_modules += [dev_debug, dev_debug_module]
