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

project(
  'Shibboleth',
  'c', 'cpp',
  version: 'dev',
  meson_version: '1.6.0',
  default_options: {
    'warning_level': '3',
    'werror': true,
    'c_std': 'c17', # Using c17 instead of c18, because when we add objc on Mac, it uses this variable to initialize objc_c_std, even though we defined it below.
    'cpp_std': 'c++20',
    'cpp_eh': 'a',
    'cpp_rtti': false,
    'objc_c_std': 'c17',
    # 'backend_startup_project': 'GameApp',
  }
)

###################
# Global Variables.
###################
compiler = meson.get_compiler('cpp')

is_clang = compiler.get_id() == 'clang'
is_msvc = compiler.get_id() == 'msvc'
is_gcc = compiler.get_id() == 'gcc'

is_windows = host_machine.system() == 'windows'
is_linux = host_machine.system() == 'linux'
is_mac = host_machine.system() == 'darwin' and host_machine.subsystem() == 'macos'

is_arm64 = host_machine.cpu_family() == 'aarch64'
is_x64 = host_machine.cpu_family() == 'x86_64'

is_debug_optimized = get_option('buildtype') == 'debugoptimized'
is_debug = is_debug_optimized or (get_option('buildtype') == 'debug')
is_release = get_option('buildtype') == 'release'
is_profile = is_debug or get_option('profile')

is_wayland = is_linux and get_option('wayland')

assert(is_windows or is_linux or is_mac, 'Unsupported OS: ' + host_machine.system())
assert(is_msvc or is_gcc or is_clang, 'Unsupported compiler: ' + compiler.get_id())
assert(is_x64 or is_arm64, 'Unsupported CPU architecture: ' + host_machine.cpu_family())
assert(not is_windows or is_msvc, 'Unsupported compiler "' + compiler.get_id() + '" on platform "' + host_machine.cpu_family() + '".')

qt = import('qt6')

declare_dependency(
  compile_args: [ '-DQT_DISABLE_DEPRECATED_BEFORE=0x060000' ],
)

###############
# Global Setup.
###############
default_cpp_args = []
default_c_args = []

add_project_arguments(
  '-DUNICODE', '-D_UNICODE',
  '-DGLM_FORCE_INTRINSICS',
  '-DZLIB_COMPAT',
  language: ['c', 'cpp']
)

if is_debug
  add_project_arguments(
    '-D_DEBUG',
    '-DDEBUG',
    language: ['c', 'cpp']
  )

elif is_release
  add_project_arguments(
    '-D_HAS_ITERATOR_DEBUGGING=0',
    '-D_ITERATOR_DEBUG_LEVEL=0',
    '-DNDEBUG',
    language: ['c', 'cpp']
  )
endif

if is_profile
  add_project_arguments(
   '-DSHIB_PROFILE',
   language: ['c', 'cpp']
  )

endif

if is_windows
  add_project_arguments(
    '-DWIN32',
    '-DWIN64',
    '-D_WINDOWS',
    language: ['c', 'cpp']
  )

elif is_mac
  add_languages('objc', native: false)
endif

if is_msvc
  add_project_arguments(
    '/Zc:__cplusplus',
    '/permissive-',
    '/Zc:static_assert-',
    '/GR-',
    '/fp:fast',
    '/Zc:wchar_t',
    language: ['c', 'cpp']
  )

else
  add_project_arguments(
    '-fchar8_t',
    '-Wno-extra-semi',
    language: ['c', 'cpp']
  )
endif

if is_x64
  add_project_arguments(
    #'-DGLM_FORCE_SSE42',
    '-DGGLM_FORCE_SILENT_WARNINGS',
    language: ['c', 'cpp']
  )
endif

###########################
# Run all meson.build files.
############################

# Dependency Folders
all_deps = []

subdir('src/Dependencies/ads')
subdir('src/Dependencies/angelscript')
subdir('src/Dependencies/assimp')
subdir('src/Dependencies/argparse')
subdir('src/Dependencies/capstone')
subdir('src/Dependencies/Catch2')
subdir('src/Dependencies/EASTL')
subdir('src/Dependencies/fmt')
subdir('src/Dependencies/glfw')
subdir('src/Dependencies/glm')
subdir('src/Dependencies/imgui')
subdir('src/Dependencies/libpng')
subdir('src/Dependencies/libtiff')
subdir('src/Dependencies/mimalloc')
subdir('src/Dependencies/minizip-ng')
subdir('src/Dependencies/mpack')
subdir('src/Dependencies/rapidjson')
subdir('src/Dependencies/zlib-ng')
subdir('src/Dependencies/zstd')

subdir('src/Dependencies/JoltPhysics')
subdir('src/Dependencies/tracy')

all_deps_target = alias_target('dependencies', all_deps)

# Frameworks
all_frameworks = []

subdir('src/Frameworks/Gaff')
subdir('src/Frameworks/Gleam')
subdir('src/Frameworks/Esprit')

all_frameworks_target = alias_target('frameworks', all_frameworks)

# Tool Folders
all_tools = []

subdir('src/Tools/CodeGenerator')
subdir('src/Tools/JSONToMessagePack')

# Core Folders
default_static_dependencies = []
default_dependencies = []

if is_profile
  default_static_dependencies += tracy_client_static_dep
  default_dependencies += tracy_client_dep
endif

all_core = []

subdir('src/Core/Memory')
subdir('src/Core/Engine')
subdir('src/Core/GameApp')

subdir('src/Tools/ReflectionHashDump') # ReflectionHashDump depends on Memory and Engine.

all_core_target = alias_target('core', all_core)
all_tools_target = alias_target('tools', all_tools)

# Module Folders
all_modules = []

subdir('src/Modules/DevDebug')

default_module_dependencies = []

if is_debug
  default_module_dependencies += dev_debug_dep
endif

subdir('src/Modules/MainLoop')
subdir('src/Modules/Resource')
subdir('src/Modules/Player')
subdir('src/Modules/Input')
subdir('src/Modules/Entity')
subdir('src/Modules/Script')
subdir('src/Modules/Logic')
subdir('src/Modules/Graphics')
subdir('src/Modules/DevDebugGraphics')
subdir('src/Modules/Physics')
subdir('src/Modules/Camera')
subdir('src/Modules/Scene')

# Editor Modules
subdir('src/Modules/DevEditor')

all_modules_target = alias_target('modules', all_modules)

# Test Folders
all_tests = []

subdir('src/Tests/AllocatorTest')
subdir('src/Tests/ReflectionTest')

all_tests_target = alias_target('tests', all_tests)

if is_windows
  qmake = find_program('qmake')
  qt_root= qmake.full_path().to_lower()

  if is_windows and qt_root.endswith('.exe')
    qt_root = qt_root.substring(0, -4)
  endif

  qt_root = qt_root.substring(0, -5) / '..'
  qt_plugins = qt_root / 'plugins'
  qt_platforms = qt_plugins / 'platforms'
  qt_bin = qt_root / 'bin'

  qt_modules = [
    'Core',
    'Gui',
    'Widgets',
  ]

  if is_windows
    extension = '.dll'
  elif is_linux
    extension = '.so'
  elif is_mac
    extension = '.dylib'
  endif

  qt_prefix = 'Qt6'

  qt_files = []

  # $TODO: Platform files
  foreach module : qt_modules
    qt_files += [
      qt_bin / (qt_prefix + module + 'd' + extension),
      qt_bin / (qt_prefix + module + extension),
    ]

    if is_windows
      qt_files += [
        qt_bin / (qt_prefix + module + 'd.pdb'),
        qt_bin / (qt_prefix + module + '.pdb'),
      ]

      install_data(
        qt_platforms / 'qwindowsd.dll',
        qt_platforms / 'qwindowsd.pdb',
        qt_platforms / 'qwindows.dll',
        qt_platforms / 'qwindows.pdb',
        install_dir: 'bin/platforms',
      )
    endif
  endforeach

  install_data(
    qt_files,
    install_dir: 'bin',
  )
endif
