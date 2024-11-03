header_files = [
  'include/GLFW/glfw3.h',
  'include/GLFW/glfw3native.h',
  'src/cocoa_joystick.h',
  'src/cocoa_platform.h',
  'src/cocoa_time.h',
  'src/internal.h',
  'src/linux_joystick.h',
  'src/mappings.h',
  'src/null_joystick.h',
  'src/null_platform.h',
  'src/platform.h',
  'src/posix_poll.h',
  'src/posix_thread.h',
  'src/posix_time.h',
  'src/win32_joystick.h',
  'src/win32_platform.h',
  'src/win32_thread.h',
  'src/win32_time.h',
  'src/wl_platform.h',
  'src/x11_platform.h',
  'src/xkb_unicode.h',
]

source_files = [
  'src/cocoa_time.c',
  'src/context.c',
  'src/egl_context.c',
  'src/glx_context.c',
  'src/init.c',
  'src/input.c',
  'src/linux_joystick.c',
  'src/monitor.c',
  'src/null_init.c',
  'src/null_joystick.c',
  'src/null_monitor.c',
  'src/null_window.c',
  'src/osmesa_context.c',
  'src/platform.c',
  'src/posix_module.c',
  'src/posix_poll.c',
  'src/posix_thread.c',
  'src/posix_time.c',
  'src/vulkan.c',
  'src/wgl_context.c',
  'src/win32_init.c',
  'src/win32_joystick.c',
  'src/win32_module.c',
  'src/win32_monitor.c',
  'src/win32_thread.c',
  'src/win32_time.c',
  'src/win32_window.c',
  'src/window.c',
  'src/wl_init.c',
  'src/wl_monitor.c',
  'src/wl_window.c',
  'src/x11_init.c',
  'src/x11_monitor.c',
  'src/x11_window.c',
  'src/xkb_unicode.c',
]

extra_files = [
]

include_dirs = [
  'include',
]

build_args = [
  '-D_GLFW_BUILD_DLL'
]

dependencies = [
]

if is_windows
  build_args += ['-D_GLFW_WIN32']

elif is_linux
  if is_wayland
    build_args += ['-D_GLFW_WAYLAND']
  else
    build_args += ['-D_GLFW_X11']
  endif

elif is_mac
  build_args += ['-D_GLFW_COCOA']
  dependencies += dependency('appleframeworks', modules: ['Cocoa', 'IOKit', 'QuartzCore'])

  source_files += [
    'src/cocoa_init.m',
    'src/cocoa_joystick.m',
    'src/cocoa_monitor.m',
    'src/cocoa_window.m',
    'src/nsgl_context.m',
  ]
endif

if is_msvc
  build_args += ['-D_CRT_SECURE_NO_WARNINGS']
endif

glfw = shared_library(
  'GLFW',
  header_files,
  source_files,
  include_directories: include_dirs,
  c_args: default_c_args + build_args,
  cpp_args: default_cpp_args + build_args,
  objc_args: build_args,
  dependencies: dependencies,
  override_options: {
    'werror': false,
    }
)

