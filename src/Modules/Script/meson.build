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
  'include/Shibboleth_AngelScriptArray.h',
  'include/Shibboleth_AngelScriptManager.h',
  'include/Shibboleth_AngelScriptMath.h',
  'include/Shibboleth_AngelScriptResource.h',
  'include/Shibboleth_AngelScriptString.h',
  'include/Shibboleth_IncludeAngelScript.h',
  'include/Shibboleth_ScriptConfig.h',
  'include/Shibboleth_ScriptDefines.h',
  'include/Shibboleth_ScriptLogging.h',
]

source_files = [
  'src/Shibboleth_AngelScriptArray.cpp',
  'src/Shibboleth_AngelScriptManager.cpp',
  'src/Shibboleth_AngelScriptMath.cpp',
  'src/Shibboleth_AngelScriptResource.cpp',
  'src/Shibboleth_AngelScriptString.cpp',
  'src/Shibboleth_ScriptConfig.cpp',
  'src/Shibboleth_ScriptModule.cpp',
]

module_source_files = [
  'src/Shibboleth_ScriptModule.cpp',
]

generated_files = [
  custom_target(
    'Gen_ReflectionInit.h',
    output: 'Gen_ReflectionInit.h',
    command: [code_generator, 'module_header', '-rp', '@SOURCE_ROOT@', '-of', '@OUTPUT@', '-m', 'Script'],
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
  angelscript_dep,

  engine_dep,
  memory_dep,

  main_loop_dep,
  resource_dep,
]

script = static_library(
  'Script',
  header_files,
  source_files,
  generated_files,
  include_directories: include_dirs,
  extra_files: extra_files,
  cpp_args: default_cpp_args + build_args,
  dependencies: default_dependencies + default_module_dependencies + dependencies,
)

script_dep = declare_dependency(
  include_directories: include_dirs,
  dependencies: dependencies,
  link_with: [script],
  sources: [generated_files],
)

script_module = shared_library(
  'ScriptModule',
  module_source_files,
  cpp_args: default_cpp_args + module_build_args,
  dependencies: script_dep,
  install: true,
  install_dir: 'Modules',
)

all_modules += [script, script_module]

