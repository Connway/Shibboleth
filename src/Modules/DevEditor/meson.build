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
  'include/Shibboleth_EditorAttributesCommon.h',
  'include/Shibboleth_EditorMainLoop.h',
  'include/Shibboleth_EditorMainWindow.h',
  'include/Shibboleth_QtReflection.h',
  'include/Shibboleth_ToDoWindow.h',
]

source_files = [
  'src/Shibboleth_DevEditorModule.cpp',
  'src/Shibboleth_EditorAttributesCommon.cpp',
  'src/Shibboleth_EditorMainLoop.cpp',
  'src/Shibboleth_EditorMainWindow.cpp',
  'src/Shibboleth_ToDoWindow.cpp',
]

module_source_files = [
  'src/Shibboleth_DevEditorModule.cpp',
]

generated_files = [
  custom_target(
    'Gen_ReflectionInit.h',
    output: 'Gen_ReflectionInit.h',
    command: [code_generator, 'module_header', '-rp', '@SOURCE_ROOT@', '-of', '@OUTPUT@', '-m', 'DevEditor'],
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
  ads_dep,

  engine_dep,
  memory_dep,

  main_loop_dep,
]

source_files += [
  qt.compile_translations(ts_files: ['ShibEd_en_US.ts']),
  qt.compile_moc(headers: header_files, dependencies: dependencies),
]

dev_editor = static_library(
  'DevEditor',
  header_files,
  source_files,
  generated_files,
  include_directories: include_dirs,
  extra_files: extra_files,
  cpp_args: default_cpp_args + build_args,
  dependencies: default_dependencies + default_module_dependencies + dependencies,
)

dev_editor_dep = declare_dependency(
  include_directories: include_dirs,
  dependencies: dependencies,
  link_with: [dev_editor],
  sources: [generated_files],
)

dev_editor_module = shared_library(
  'DevEditorModule',
  module_source_files,
  cpp_args: default_cpp_args + module_build_args,
  dependencies: dev_editor_dep,
  install: true,
  install_dir: 'DevModules',
)

all_modules += [dev_editor, dev_editor_module]

