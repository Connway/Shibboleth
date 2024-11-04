header_files = [
  'imconfig.h',
  'imgui.h',
  'imgui_internal.h',
  'imstb_rectpack.h',
  'imstb_textedit.h',
  'imstb_truetype.h',
  'misc/cpp/imgui_stdlib.h',
  'misc/freetype/imgui_freetype.h',
  'misc/single_file/imgui_single_file.h',
]

source_files = [
  'imgui.cpp',
  'imgui_demo.cpp',
  'imgui_draw.cpp',
  'imgui_tables.cpp',
  'imgui_widgets.cpp',
  'misc/cpp/imgui_stdlib.cpp',
  'misc/fonts/binary_to_compressed_c.cpp',
]

extra_files = [
  'misc/debuggers/imgui.natvis',
]

include_dirs = [
]

build_args = [
]

imgui = static_library(
  'imgui',
  header_files,
  source_files,
  include_directories: include_dirs,
  cpp_args: default_cpp_args + build_args,
  extra_files: extra_files,
)

imgui_deps = declare_dependency(
  include_directories: ['.'],
  link_with: [imgui],
)

all_deps += imgui
