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
  'rapidjson/allocators.h',
  'rapidjson/cursorstreamwrapper.h',
  'rapidjson/document.h',
  'rapidjson/encodedstream.h',
  'rapidjson/encodings.h',
  'rapidjson/error/en.h',
  'rapidjson/error/error.h',
  'rapidjson/filereadstream.h',
  'rapidjson/filewritestream.h',
  'rapidjson/fwd.h',
  'rapidjson/internal/biginteger.h',
  'rapidjson/internal/clzll.h',
  'rapidjson/internal/diyfp.h',
  'rapidjson/internal/dtoa.h',
  'rapidjson/internal/ieee754.h',
  'rapidjson/internal/itoa.h',
  'rapidjson/internal/meta.h',
  'rapidjson/internal/pow10.h',
  'rapidjson/internal/regex.h',
  'rapidjson/internal/stack.h',
  'rapidjson/internal/strfunc.h',
  'rapidjson/internal/strtod.h',
  'rapidjson/internal/swap.h',
  'rapidjson/istreamwrapper.h',
  'rapidjson/memorybuffer.h',
  'rapidjson/memorystream.h',
  'rapidjson/msinttypes/inttypes.h',
  'rapidjson/msinttypes/stdint.h',
  'rapidjson/ostreamwrapper.h',
  'rapidjson/pointer.h',
  'rapidjson/prettywriter.h',
  'rapidjson/rapidjson.h',
  'rapidjson/reader.h',
  'rapidjson/schema.h',
  'rapidjson/stream.h',
  'rapidjson/stringbuffer.h',
  'rapidjson/uri.h',
  'rapidjson/writer.h',
]

source_files = [
]

extra_files = [
  'rapidjson/contrib/natvis/rapidjson.natvis',
]

include_dirs = [
]

build_args = [
]

# rapidjson = static_library(
#   'rapidjson',
#   header_files,
#   source_files,
#   include_directories: include_dirs,
#   cpp_args: default_cpp_args + build_args,
#   extra_files: extra_files,
# )

rapidjson_dep = declare_dependency(
  include_directories: ['.'],
  extra_files: extra_files,
)
