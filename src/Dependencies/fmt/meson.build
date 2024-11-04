header_files = [
  'include/fmt/args.h',
  'include/fmt/chrono.h',
  'include/fmt/color.h',
  'include/fmt/compile.h',
  'include/fmt/core.h',
  'include/fmt/format-inl.h',
  'include/fmt/format.h',
  'include/fmt/os.h',
  'include/fmt/ostream.h',
  'include/fmt/printf.h',
  'include/fmt/ranges.h',
  'include/fmt/std.h',
  'include/fmt/xchar.h',
]

source_files = [
  # 'src/fmt.cc',
  'src/format.cc',
  'src/os.cc',
]

extra_files = [
]

include_dirs = [
  'include',
]

build_args = [
]

fmt = static_library(
  'fmt',
  header_files,
  source_files,
  include_directories: include_dirs,
  c_args: default_c_args + build_args,
)

fmt_no_link_dep = declare_dependency(
  include_directories: include_dirs,
)

fmt_dep = declare_dependency(
  dependencies: fmt_no_link_dep,
  link_with: [fmt],
)

all_deps += fmt
