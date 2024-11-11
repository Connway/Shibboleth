header_files = [
  'include/Shibboleth_DeferredResourcePtr.h',
  'include/Shibboleth_DeferredResourcePtrReflection.h',
  'include/Shibboleth_IResource.h',
  'include/Shibboleth_ResourceAttributesCommon.h',
  'include/Shibboleth_ResourceDefines.h',
  'include/Shibboleth_ResourceLogging.h',
  'include/Shibboleth_ResourceManager.h',
  'include/Shibboleth_ResourcePtr.h',
  'include/Shibboleth_ResourcePtrReflection.h',
  'include/Shibboleth_ResourceSystem.h',
]

source_files = [
  'src/Shibboleth_DeferredResourcePtr.cpp',
  'src/Shibboleth_IResource.cpp',
  'src/Shibboleth_ResourceAttributesCommon.cpp',
  'src/Shibboleth_ResourceManager.cpp',
  'src/Shibboleth_ResourceModule.cpp',
  'src/Shibboleth_ResourcePtrReflection.cpp',
  'src/Shibboleth_ResourceSystem.cpp',
]

module_source_files = [
  'src/Shibboleth_ResourceModule.cpp',
]

generated_files = [
  custom_target(
    'Gen_ReflectionInit.h',
    output: 'Gen_ReflectionInit.h',
    command: [code_generator, 'module_header', '-rp', '@SOURCE_ROOT@', '-of', '@OUTPUT@', '-m', 'Resource'],
    depends: [code_generator],
    build_always_stale: true,
  ),
]

extra_files = [
  'include/Shibboleth_DeferredResourcePtrReflection.inl',
  'include/Shibboleth_ResourcePtrReflection.inl',
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
]

resource = static_library(
  'Resource',
  header_files,
  source_files,
  generated_files,
  include_directories: include_dirs,
  extra_files: extra_files,
  cpp_args: default_cpp_args + build_args,
  dependencies: default_dependencies + default_module_dependencies + dependencies,
)

resource_dep = declare_dependency(
  include_directories: include_dirs,
  dependencies: dependencies,
  link_with: [resource],
  sources: [generated_files],
)

resource_module = shared_library(
  'ResourceModule',
  module_source_files,
  cpp_args: default_cpp_args + module_build_args,
  dependencies: resource_dep,
)

all_modules += [resource, resource_module]

