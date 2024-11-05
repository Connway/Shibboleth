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

if is_windows
  dependencies += [
    compiler.find_library('Dbghelp'),
    compiler.find_library('iphlpapi'),
    compiler.find_library('psapi'),
    compiler.find_library('userenv'),
  ]
endif

game_app = executable(
  'GameApp',
  header_files,
  source_files,
  include_directories: include_dirs,
  cpp_args: default_cpp_args + build_args,
  dependencies: dependencies,
  win_subsystem: 'windows',
)

all_core += game_app
