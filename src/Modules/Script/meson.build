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

