header_files = [
  'include/Shibboleth_Layer.h',
  'include/Shibboleth_LayerResource.h',
  'include/Shibboleth_Scene.h',
  'include/Shibboleth_SceneConfig.h',
  'include/Shibboleth_SceneDefines.h',
  'include/Shibboleth_SceneLogging.h',
  'include/Shibboleth_SceneManager.h',
  'include/Shibboleth_SceneResource.h',
]

source_files = [
  'src/Shibboleth_Layer.cpp',
  'src/Shibboleth_LayerResource.cpp',
  'src/Shibboleth_Scene.cpp',
  'src/Shibboleth_SceneConfig.cpp',
  'src/Shibboleth_SceneManager.cpp',
  'src/Shibboleth_SceneModule.cpp',
  'src/Shibboleth_SceneResource.cpp',
]

module_source_files = [
  'src/Shibboleth_SceneModule.cpp',
]

generated_files = [
  custom_target(
    'Gen_ReflectionInit.h',
    output: 'Gen_ReflectionInit.h',
    command: [code_generator, 'module_header', '-rp', '@SOURCE_ROOT@', '-of', '@OUTPUT@', '-m', 'Scene'],
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

  resource_dep,
  entity_dep,
]

scene = static_library(
  'Scene',
  header_files,
  source_files,
  generated_files,
  include_directories: include_dirs,
  extra_files: extra_files,
  cpp_args: default_cpp_args + build_args,
  dependencies: default_dependencies + default_module_dependencies + dependencies,
)

scene_dep = declare_dependency(
  include_directories: include_dirs,
  dependencies: dependencies,
  link_with: [scene],
  sources: [generated_files],
)

scene_module = shared_library(
  'SceneModule',
  module_source_files,
  cpp_args: default_cpp_args + module_build_args,
  dependencies: scene_dep,
)

all_modules += [scene, scene_module]

