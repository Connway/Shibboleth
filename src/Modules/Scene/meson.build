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
  install: true,
  install_dir: 'Modules',
)

all_modules += [scene, scene_module]

