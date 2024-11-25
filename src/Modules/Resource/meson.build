#####################################################################################
# Copyright (C) by Nicholas LaCroix
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
# THE SOFTWARE.
#####################################################################################

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
    build_by_default: true,
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
  install: true,
  install_dir: 'Modules',
)

all_modules += [resource, resource_module]

