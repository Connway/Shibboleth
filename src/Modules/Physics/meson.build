header_files = [
  'include/Shibboleth_PhysicsManager.h',
  'include/Shibboleth_PhysicsMaterialResource.h',
  'include/Shibboleth_PhysicsShapeResource.h',
  'include/Shibboleth_PhysicsSystem.h',
  'include/Shibboleth_RigidBodyComponent.h',
]

source_files = [
  'src/Shibboleth_PhysicsManager.cpp',
  'src/Shibboleth_PhysicsMaterialResource.cpp',
  'src/Shibboleth_PhysicsModule.cpp',
  'src/Shibboleth_PhysicsShapeResource.cpp',
  'src/Shibboleth_PhysicsSystem.cpp',
  'src/Shibboleth_RigidBodyComponent.cpp',
]

module_source_files = [
  'src/Shibboleth_PhysicsModule.cpp',
]

generated_files = [
  custom_target(
    'Gen_ReflectionInit.h',
    output: 'Gen_ReflectionInit.h',
    command: [code_generator, 'module_header', '-rp', '@SOURCE_ROOT@', '-of', '@OUTPUT@', '-m', 'Physics'],
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
  jolt_physics_dep,

  engine_dep,
  memory_dep,

  main_loop_dep,
  resource_dep,
]

if is_debug
  dependencies += dev_debug_graphics_dep
endif

physics = static_library(
  'Physics',
  header_files,
  source_files,
  generated_files,
  include_directories: include_dirs,
  extra_files: extra_files,
  cpp_args: default_cpp_args + build_args,
  dependencies: default_dependencies + default_module_dependencies + dependencies,
)

physics_dep = declare_dependency(
  include_directories: include_dirs,
  dependencies: dependencies,
  link_with: [physics],
  sources: [generated_files],
)

physics_module = shared_library(
  'PhysicsModule',
  module_source_files,
  cpp_args: default_cpp_args + module_build_args,
  dependencies: physics_dep,
  install: true,
  install_dir: 'Modules',
)

all_modules += [physics, physics_module]

