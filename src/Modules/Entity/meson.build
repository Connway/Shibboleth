header_files = [
  'include/Shibboleth_Entity.h',
  'include/Shibboleth_EntityComponent.h',
  'include/Shibboleth_EntityDefines.h',
  'include/Shibboleth_EntityManager.h',
  'include/Shibboleth_EntityResource.h',
  'include/Shibboleth_EntitySceneComponent.h',
  'include/Shibboleth_EntitySystem.h',
  'include/Shibboleth_EntityUpdater.h',
]

source_files = [
  'src/Shibboleth_Entity.cpp',
  'src/Shibboleth_EntityComponent.cpp',
  'src/Shibboleth_EntityManager.cpp',
  'src/Shibboleth_EntityModule.cpp',
  'src/Shibboleth_EntityResource.cpp',
  'src/Shibboleth_EntitySceneComponent.cpp',
  'src/Shibboleth_EntitySystem.cpp',
  'src/Shibboleth_EntityUpdater.cpp',
]

module_source_files = [
  'src/Shibboleth_EntityModule.cpp',
]

generated_files = [
  custom_target(
    'Gen_ReflectionInit.h',
    output: 'Gen_ReflectionInit.h',
    command: [code_generator, 'module_header', '-rp', '@SOURCE_ROOT@', '-of', '@OUTPUT@', '-m', 'Entity'],
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
  gleam_dep,

  engine_dep,
  memory_dep,

  resource_dep,
]

entity = static_library(
  'Entity',
  header_files,
  source_files,
  generated_files,
  include_directories: include_dirs,
  cpp_args: default_cpp_args + build_args,
  dependencies: default_dependencies + dependencies,
)

entity_dep = declare_dependency(
  include_directories: include_dirs,
  dependencies: dependencies,
  link_with: [entity],
  sources: [generated_files],
)

entity_module = shared_library(
  'EntityModule',
  module_source_files,
  cpp_args: default_cpp_args + module_build_args,
  dependencies: entity_dep,
)

all_modules += [entity, entity_module]

