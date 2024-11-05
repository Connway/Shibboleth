header_files = [
]

source_files = [
  'Game_Main.cpp',
]

extra_files = [
]

include_dirs = [
]

build_args = [
]

dependencies = [
  engine_dep,
  memory_dep,
]

game_app = executable(
  'GameApp',
  header_files,
  source_files,
  include_directories: include_dirs,
  cpp_args: default_cpp_args + build_args,
  dependencies: dependencies,
  win_subsystem: 'windows',
)

# game_app_console = executable(
#   'GameAppConsole',
#   header_files,
#   source_files,
#   include_directories: include_dirs,
#   cpp_args: default_cpp_args + build_args,
#   dependencies: dependencies,
#   win_subsystem: 'console',
# )

# all_core += [game_app, game_app_console]
all_core += game_app
