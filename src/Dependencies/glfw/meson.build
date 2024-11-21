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
  },
  install: true,
  install_dir: 'bin',
)

glfw_dep = declare_dependency(
  include_directories: include_dirs,
  link_with: [glfw],
)

all_deps += glfw
