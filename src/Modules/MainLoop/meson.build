header_files = [
  'include/Shibboleth_GameTime.h',
  'include/Shibboleth_GameTimeSystem.h',
  'include/Shibboleth_ISystem.h',
  'include/Shibboleth_MainLoop.h',
  'include/Shibboleth_UpdatePhases.h',
]

source_files = [
  'src/Shibboleth_GameTime.cpp',
  'src/Shibboleth_GameTimeSystem.cpp',
  'src/Shibboleth_ISystem.cpp',
  'src/Shibboleth_MainLoop.cpp',
  'src/Shibboleth_MainLoopModule.cpp',
  'src/Shibboleth_UpdatePhases.cpp',
]

module_source_files = [
  'src/Shibboleth_MainLoopModule.cpp',
]

generated_files = [
  custom_target(
    'Gen_ReflectionInit.h',
    output: 'Gen_ReflectionInit.h',
    command: [code_generator, 'module_header', '-rp', '@SOURCE_ROOT@', '-of', '@OUTPUT@', '-m', 'MainLoop'],
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

main_loop = static_library(
  'MainLoop',
  header_files,
  source_files,
  generated_files,
  include_directories: include_dirs,
  cpp_args: default_cpp_args + build_args,
  dependencies: dependencies,
)

main_loop_dep = declare_dependency(
  include_directories: include_dirs,
  dependencies: dependencies,
  link_with: [main_loop],
  sources: [generated_files],
)

main_loop_module = shared_library(
  'MainLoopModule',
  module_source_files,
  cpp_args: default_cpp_args + module_build_args,
  dependencies: main_loop_dep,
)

all_modules += [main_loop, main_loop_module]
