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
    build_by_default: true,
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
  install: true,
  install_dir: 'Modules',
)

all_modules += [camera, camera_module]

