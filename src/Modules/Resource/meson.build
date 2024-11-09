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

extra_files = [
  'include/Shibboleth_DeferredResourcePtrReflection.inl',
  'include/Shibboleth_ResourcePtrReflection.inl',
]

include_dirs = [
  'include',
]

build_args = [
]

dependencies = [
  engine_dep,
  memory_dep,

  main_loop_dep,
]

if is_debug
  dependencies += dev_debug_dep
endif

game_app = executable(
  'GameApp',
  header_files,
  source_files,
  include_directories: include_dirs,
  extra_files: extra_files,
  cpp_args: default_cpp_args + build_args,
  dependencies: default_dependencies + dependencies,
)

all_core += game_app

