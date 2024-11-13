header_files = [
  'include/Shibboleth_InputActivator.h',
  'include/Shibboleth_InputManager.h',
  'include/Shibboleth_InputManagerOld.h',
  'include/Shibboleth_InputMapping.h',
  'include/Shibboleth_InputMappingResource.h',
  'include/Shibboleth_InputReflection.h',
  'include/Shibboleth_InputSystem.h',
  'include/Shibboleth_PlayerInputSubsystem.h',
]

source_files = [
  'src/Shibboleth_InputActivator.cpp',
  'src/Shibboleth_InputManager.cpp',
  'src/Shibboleth_InputManagerOld.cpp',
  'src/Shibboleth_InputMapping.cpp',
  'src/Shibboleth_InputMappingResource.cpp',
  'src/Shibboleth_InputModule.cpp',
  'src/Shibboleth_InputReflection.cpp',
  'src/Shibboleth_InputSystem.cpp',
  'src/Shibboleth_PlayerInputSubsystem.cpp',
]

module_source_files = [
  'src/Shibboleth_InputModule.cpp',
]

generated_files = [
  custom_target(
    'Gen_ReflectionInit.h',
    output: 'Gen_ReflectionInit.h',
    command: [code_generator, 'module_header', '-rp', '@SOURCE_ROOT@', '-of', '@OUTPUT@', '-m', 'Input'],
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

  main_loop_dep,
  resource_dep,
  player_dep,
]

input = static_library(
  'Input',
  header_files,
  source_files,
  generated_files,
  include_directories: include_dirs,
  extra_files: extra_files,
  cpp_args: default_cpp_args + build_args,
  dependencies: default_dependencies + default_module_dependencies + dependencies,
)

input_dep = declare_dependency(
  include_directories: include_dirs,
  dependencies: dependencies,
  link_with: [input],
  sources: [generated_files],
)

input_module = shared_library(
  'InputModule',
  module_source_files,
  cpp_args: default_cpp_args + module_build_args,
  dependencies: input_dep,
  install: true,
  install_dir: 'Modules',
)

all_modules += [input, input_module]

