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

