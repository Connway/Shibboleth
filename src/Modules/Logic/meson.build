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
  'include/Shibboleth_AngelScriptProcess.h',
  'include/Shibboleth_StateMachineComponent.h',
  'include/Shibboleth_StateMachineReflection.h',
  'include/Shibboleth_StateMachineResource.h',
  'include/Shibboleth_VariableSet.h',
]

source_files = [
  'src/Shibboleth_AngelScriptProcess.cpp',
  'src/Shibboleth_LogicModule.cpp',
  'src/Shibboleth_StateMachineComponent.cpp',
  'src/Shibboleth_StateMachineReflection.cpp',
  'src/Shibboleth_StateMachineResource.cpp',
  'src/Shibboleth_VariableSet.cpp',
]

module_source_files = [
  'src/Shibboleth_LogicModule.cpp',
]

generated_files = [
  custom_target(
    'Gen_ReflectionInit.h',
    output: 'Gen_ReflectionInit.h',
    command: [code_generator, 'module_header', '-rp', '@SOURCE_ROOT@', '-of', '@OUTPUT@', '-m', 'Logic'],
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

  main_loop_dep,
  resource_dep,
  script_dep,
  entity_dep,
]

logic = static_library(
  'Logic',
  header_files,
  source_files,
  generated_files,
  include_directories: include_dirs,
  extra_files: extra_files,
  cpp_args: default_cpp_args + build_args,
  dependencies: default_dependencies + default_module_dependencies + dependencies,
)

logic_dep = declare_dependency(
  include_directories: include_dirs,
  dependencies: dependencies,
  link_with: [logic],
  sources: [generated_files],
)

logic_module = shared_library(
  'LogicModule',
  module_source_files,
  cpp_args: default_cpp_args + module_build_args,
  dependencies: logic_dep,
  install: true,
  install_dir: 'Modules',
)

all_modules += [logic, logic_module]

