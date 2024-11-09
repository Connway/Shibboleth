header_files = [
  'include/Shibboleth_DebugManager.h',
  'include/Shibboleth_DebugSystem.h',
  'include/Shibboleth_IDebugManager.h',
]

source_files = [
  'src/Shibboleth_DebugManager.cpp',
  'src/Shibboleth_DebugSystem.cpp',
]

module_source_files = [
  'src/Shibboleth_DevDebugGraphicsModule.cpp',
]

generated_files = [
  custom_target(
    'Gen_ReflectionInit.h',
    output: 'Gen_ReflectionInit.h',
    command: [code_generator, 'module_header', '-rp', '@SOURCE_ROOT@', '-of', '@OUTPUT@', '-m', 'DevDebugGraphics'],
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

  dev_debug_dep,
  graphics_dep,
]

dev_debug_graphics = static_library(
  'DevDebugGraphics',
  header_files,
  source_files,
  generated_files,
  include_directories: include_dirs,
  cpp_args: default_cpp_args + build_args,
  dependencies: default_dependencies + dependencies,
)

dev_debug_graphics_dep = declare_dependency(
  include_directories: include_dirs,
  dependencies: dependencies,
  link_with: [dev_debug_graphics],
  sources: [generated_files],
)

dev_debug_graphics_module = shared_library(
  'DevDebugGraphicsModule',
  module_source_files,
  cpp_args: default_cpp_args + module_build_args,
  dependencies: dev_debug_graphics_dep,
)

all_modules += [dev_debug_graphics, dev_debug_graphics_module]

