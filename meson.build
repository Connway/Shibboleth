project(
  'Shibboleth',
  'c', 'cpp',
  version: 'dev',
  meson_version: '1.6.0',
  default_options: [
    'warning_level=3',
    'c_std=c18',
    'cpp_std=c++20',
    'cpp_eh=a',
    'cpp_rtti=false',
  ]
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

if is_msvc
  default_cpp_args = default_cpp_args + ['/WX']

  add_project_arguments(
    '-DWIN32',
    '-DWIN64',
    '-D_WINDOWS',
    '/Zc:__cplusplus',
    '/permissive-',
    '/Zc:static_assert-',
    '/GR-',
    '/fp:fast',
    '/Zc:wchar_t',
    language: ['c', 'cpp']
  )

else
  default_cpp_args = default_cpp_args + ['-Werror']

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

##########################################
# Grab all folders with meson.build files.
##########################################
module_folders = run_command('python3', 'utils/find_projects.py', 'src/Modules', check: true).stdout().strip().split(',')
tool_folders = run_command('python3', 'utils/find_projects.py', 'src/Tools', check: true).stdout().strip().split(',')
core_folders = run_command('python3', 'utils/find_projects.py', 'src/Core', check: true).stdout().strip().split(',')
test_folders = run_command('python3', 'utils/find_projects.py', 'src/Test', check: true).stdout().strip().split(',')

# If stdout is an empty string, split() will generate ['']. Convert those into empty arrays.
if module_folders[0] == ''
  module_folders = []
endif

if tool_folders[0] == ''
  tool_folders = []
endif

if core_folders[0] == ''
  core_folders = []
endif

if test_folders[0] == ''
  test_folders = []
endif



############################
# Run all meson.build files.
############################

# Dependency Folders


# Frameworks
subdir('src/Frameworks/Gaff')
subdir('src/Frameworks/Gleam')
subdir('src/Frameworks/Esprit')

# Core Folders
foreach build_subdir : core_folders
  subdir(build_subdir)
endforeach

# Module Folders
foreach build_subdir : module_folders
  subdir(build_subdir)
endforeach

# Tool Folders
foreach build_subdir : tool_folders
  subdir(build_subdir)
endforeach

# Test Folders
foreach build_subdir : test_folders
  subdir(build_subdir)
endforeach

