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

imgui_dep = declare_dependency(
  include_directories: ['.'],
  link_with: [imgui],
)

all_deps += imgui
