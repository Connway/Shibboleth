header_files = [
  'include/Shibboleth_CameraComponent.h',
  'include/Shibboleth_FlyCameraProcess.h',
]

source_files = [
  'src/Shibboleth_CameraComponent.cpp',
  'src/Shibboleth_CameraModule.cpp',
  'src/Shibboleth_FlyCameraProcess.cpp',
]

module_source_files = [
  'src/Shibboleth_CameraModule.cpp',
]

generated_files = [
  custom_target(
    'Gen_ReflectionInit.h',
    output: 'Gen_ReflectionInit.h',
    command: [code_generator, 'module_header', '-rp', '@SOURCE_ROOT@', '-of', '@OUTPUT@', '-m', 'Camera'],
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
  esprit_dep,

  engine_dep,
  memory_dep,

  resource_dep,
  graphics_dep,
  entity_dep,
  input_dep,
  logic_dep,
]

if is_debug
  dependencies += [dev_debug_dep]
endif

camera = static_library(
  'Camera',
  header_files,
  source_files,
  generated_files,
  include_directories: include_dirs,
  cpp_args: default_cpp_args + build_args,
  dependencies: default_dependencies + dependencies,
)

camera_dep = declare_dependency(
  include_directories: include_dirs,
  dependencies: dependencies,
  link_with: [camera],
  sources: [generated_files],
)

camera_module = shared_library(
  'CameraModule',
  module_source_files,
  cpp_args: default_cpp_args + module_build_args,
  dependencies: camera_dep,
)

all_modules += [camera, camera_module]

