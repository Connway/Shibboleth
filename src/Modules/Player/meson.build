header_files = [
  'include/Shibboleth_LocalPlayer.h',
  'include/Shibboleth_LocalPlayerSubsystem.h',
  'include/Shibboleth_Player.h',
  'include/Shibboleth_PlayerManager.h',
  'include/Shibboleth_PlayerSubsystem.h',
]

source_files = [
  'src/Shibboleth_LocalPlayer.cpp',
  'src/Shibboleth_LocalPlayerSubsystem.cpp',
  'src/Shibboleth_Player.cpp',
  'src/Shibboleth_PlayerManager.cpp',
  'src/Shibboleth_PlayerModule.cpp',
  'src/Shibboleth_PlayerSubsystem.cpp',
]

module_source_files = [
  'src/Shibboleth_PlayerModule.cpp',
]

generated_files = [
  custom_target(
    'Gen_ReflectionInit.h',
    output: 'Gen_ReflectionInit.h',
    command: [code_generator, 'module_header', '-rp', '@SOURCE_ROOT@', '-of', '@OUTPUT@', '-m', 'Player'],
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
]

player = static_library(
  'Player',
  header_files,
  source_files,
  generated_files,
  include_directories: include_dirs,
  extra_files: extra_files,
  cpp_args: default_cpp_args + build_args,
  dependencies: default_dependencies + default_module_dependencies + dependencies,
)

player_dep = declare_dependency(
  include_directories: include_dirs,
  dependencies: dependencies,
  link_with: [player],
  sources: [generated_files],
)

player_module = shared_library(
  'PlayerModule',
  module_source_files,
  cpp_args: default_cpp_args + module_build_args,
  dependencies: player_dep,
  install: true,
  install_dir: 'Modules',
)

all_modules += [player, player_module]

