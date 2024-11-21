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
  install: true,
  install_dir: 'Modules',
)

all_modules += [entity, entity_module]

