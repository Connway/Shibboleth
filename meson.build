project(
  'Shibboleth',
  'c', 'cpp',
  version: 'dev',
  meson_version: '1.6.0',
  default_options: {
    'warning_level': '3',
    'werror': true,
    'c_std': 'c18',
    'cpp_std': 'c++20',
    'cpp_eh': 'a',
    'cpp_rtti': false,
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

if not is_windows and not is_linux and not is_mac
  error('Unsupported OS:', host_machine.system())
endif

if not is_clang and not is_msvc and not is_gcc
  error('Unsupported compiler:', compiler.get_id())
endif

if not is_arm64 and not is_x64
  error('Unsupported CPU architecture:', host_machine.cpu_family())
endif

###############
# Global Setup.
###############
default_cpp_args = []

add_project_arguments(
  '-DUNICODE', '-D_UNICODE',
  '-DEA_COMPILER_NO_RTTI',
  '-DGLM_FORCE_INTRINSICS',
  '-DZLIB_COMPAT',
  '-DJPH_USE_LZCNT',
  '-DJPH_USE_TZCNT',
  '-DJPH_USE_FMADD',
  language: ['c', 'cpp']
)

if is_debug
  add_project_arguments(
    '-D__DEBUG',
    '-D_DEBUG',
    '-D_JPH_DEBUG_RENDERER',
    '-D_JPH_FLOATING_POINT_EXCEPTIONS_ENABLED',
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
   '-D_SHIB_PROFILE',
   '-D_JPH_PROFILE_ENABLED',
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
    #'GLM_FORCE_SSE42',
    #'JPH_USE_LZCNT',
    #'JPH_USE_TZCNT',
    #'JPH_USE_FMADD',
    'JPH_USE_SSE4_1',
    'JPH_USE_SSE4_2',
    #'JPH_USE_AVX2',
    #'JPH_USE_AVX',
    #'JPH_USE_F16C',
    'GLM_FORCE_SILENT_WARNINGS',
    language: ['c', 'cpp']
  )
endif

###########################
# Run all meson.build files.
############################

# Dependency Folders
subdir('src/Dependencies/Catch2')
subdir('src/Dependencies/EASTL')
subdir('src/Dependencies/JoltPhysics')
subdir('src/Dependencies/angelscript')

# Frameworks
subdir('src/Frameworks/Gaff')
subdir('src/Frameworks/Gleam')
subdir('src/Frameworks/Esprit')

# Core Folders

# Module Folders

# Tool Folders

# Test Folders

