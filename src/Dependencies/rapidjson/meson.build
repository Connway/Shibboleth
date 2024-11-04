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
