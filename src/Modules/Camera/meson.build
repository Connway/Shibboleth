header_files = [
  'include/Shibboleth_CameraComponent.h',
  'include/Shibboleth_FlyCameraProcess.h',
]

source_files = [
  'src/Shibboleth_CameraComponent.cpp',
  'src/Shibboleth_CameraModule.cpp',
  'src/Shibboleth_FlyCameraProcess.cpp',
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
]

dependencies = [
  esprit_dep,

  engine_dep,
  memory_dep,
]

camera_lib = static_library(
  'Camera',
  header_files,
  source_files,
  generated_files,
  include_directories: include_dirs,
  cpp_args: default_cpp_args + build_args,
  dependencies: dependencies,
)

# $TODO: camera_module

all_modules += [camera_lib, ]

