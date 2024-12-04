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
  'add_on/autowrapper/aswrappedcall.h',
  'add_on/contextmgr/contextmgr.h',
  'add_on/datetime/datetime.h',
  'add_on/debugger/debugger.h',
  'add_on/scriptany/scriptany.h',
  'add_on/scriptarray/scriptarray.h',
  'add_on/scriptbuilder/scriptbuilder.h',
  'add_on/scriptdictionary/scriptdictionary.h',
  'add_on/scriptfile/scriptfile.h',
  'add_on/scriptfile/scriptfilesystem.h',
  'add_on/scriptgrid/scriptgrid.h',
  'add_on/scripthandle/scripthandle.h',
  'add_on/scripthelper/scripthelper.h',
  'add_on/scriptmath/scriptmath.h',
  'add_on/scriptmath/scriptmathcomplex.h',
  'add_on/scriptsocket/scriptsocket.h',
  'add_on/scriptstdstring/scriptstdstring.h',
  'add_on/serializer/serializer.h',
  'add_on/weakref/weakref.h',
  'angelscript/include/angelscript.h',
  'angelscript/source/as_array.h',
  'angelscript/source/as_atomic.h',
  'angelscript/source/as_builder.h',
  'angelscript/source/as_bytecode.h',
  'angelscript/source/as_callfunc.h',
  'angelscript/source/as_compiler.h',
  'angelscript/source/as_config.h',
  'angelscript/source/as_configgroup.h',
  'angelscript/source/as_context.h',
  'angelscript/source/as_criticalsection.h',
  'angelscript/source/as_datatype.h',
  'angelscript/source/as_debug.h',
  'angelscript/source/as_gc.h',
  'angelscript/source/as_generic.h',
  'angelscript/source/as_map.h',
  'angelscript/source/as_memory.h',
  'angelscript/source/as_module.h',
  'angelscript/source/as_namespace.h',
  'angelscript/source/as_objecttype.h',
  'angelscript/source/as_outputbuffer.h',
  'angelscript/source/as_parser.h',
  'angelscript/source/as_property.h',
  'angelscript/source/as_restore.h',
  'angelscript/source/as_scriptcode.h',
  'angelscript/source/as_scriptengine.h',
  'angelscript/source/as_scriptfunction.h',
  'angelscript/source/as_scriptnode.h',
  'angelscript/source/as_scriptobject.h',
  'angelscript/source/as_string.h',
  'angelscript/source/as_string_util.h',
  'angelscript/source/as_symboltable.h',
  'angelscript/source/as_texts.h',
  'angelscript/source/as_thread.h',
  'angelscript/source/as_tokendef.h',
  'angelscript/source/as_tokenizer.h',
  'angelscript/source/as_typeinfo.h',
  'angelscript/source/as_variablescope.h',
]

source_files = [
  'add_on/scriptarray/scriptarray.cpp',
  'add_on/scriptbuilder/scriptbuilder.cpp',
  'angelscript/source/as_atomic.cpp',
  'angelscript/source/as_builder.cpp',
  'angelscript/source/as_bytecode.cpp',
  'angelscript/source/as_callfunc.cpp',
  'angelscript/source/as_callfunc_arm.cpp',
  'angelscript/source/as_callfunc_arm64.cpp',
  'angelscript/source/as_callfunc_mips.cpp',
  'angelscript/source/as_callfunc_ppc.cpp',
  'angelscript/source/as_callfunc_ppc_64.cpp',
  'angelscript/source/as_callfunc_riscv64.cpp',
  'angelscript/source/as_callfunc_sh4.cpp',
  'angelscript/source/as_callfunc_x64_gcc.cpp',
  'angelscript/source/as_callfunc_x64_mingw.cpp',
  'angelscript/source/as_callfunc_x64_msvc.cpp',
  'angelscript/source/as_callfunc_x86.cpp',
  'angelscript/source/as_callfunc_xenon.cpp',
  'angelscript/source/as_compiler.cpp',
  'angelscript/source/as_configgroup.cpp',
  'angelscript/source/as_context.cpp',
  'angelscript/source/as_datatype.cpp',
  'angelscript/source/as_gc.cpp',
  'angelscript/source/as_generic.cpp',
  'angelscript/source/as_globalproperty.cpp',
  'angelscript/source/as_memory.cpp',
  'angelscript/source/as_module.cpp',
  'angelscript/source/as_objecttype.cpp',
  'angelscript/source/as_outputbuffer.cpp',
  'angelscript/source/as_parser.cpp',
  'angelscript/source/as_restore.cpp',
  'angelscript/source/as_scriptcode.cpp',
  'angelscript/source/as_scriptengine.cpp',
  'angelscript/source/as_scriptfunction.cpp',
  'angelscript/source/as_scriptnode.cpp',
  'angelscript/source/as_scriptobject.cpp',
  'angelscript/source/as_string.cpp',
  'angelscript/source/as_string_util.cpp',
  'angelscript/source/as_thread.cpp',
  'angelscript/source/as_tokenizer.cpp',
  'angelscript/source/as_typeinfo.cpp',
  'angelscript/source/as_variablescope.cpp',
]

extra_files = [
]

include_dirs = [
  'angelscript/include',
  'add_on',
]

build_args = [
]

angelscript_defines = [
  '-DAS_NO_EXCEPTIONS'
]

if is_debug
  angelscript_defines += ['-DAS_DEBUG']
endif

if not is_msvc
	build_args += '-fno-strict-aliasing'
endif

if is_windows
  ml64 = find_program('ml64')
  file_no_extension = ''

  if is_x64
    file_no_extension = 'as_callfunc_x64_msvc_asm'
  elif is_arm64
    file_no_extension = 'as_callfunc_arm64_msvc'
  endif

  asm_file = 'angelscript/source/' + file_no_extension + '.asm'
  obj_name = file_no_extension + '.obj'

  assembler_args = [ml64, '/c', '/nologo', '/Fo@OUTPUT@', '/W3', '/errorReport:prompt', '@INPUT@']

  if is_debug
    assembler_args += ['/Zi', '/Zd']
  endif

  asm_target = custom_target(
    obj_name,
    input: asm_file,
    output: obj_name,
    command: assembler_args,
  )

  source_files += asm_target

elif is_linux
  if is_x64
  elif is_arm64
    if is_clang
    elif is_gcc
      source_files += ['angelscript/source/as_callfunc_arm64_gcc.S']
    endif
  endif
elif is_mac
  source_files += ['angelscript/source/as_callfunc_arm64_xcode.S']
endif

angelscript = static_library(
  'AngelScript',
  header_files,
  source_files,
  include_directories: include_dirs,
  cpp_args: default_cpp_args + build_args,
  override_options: {
    'werror': false
  }
)

angelscript_dep = declare_dependency(
  include_directories: include_dirs,
  compile_args: angelscript_defines,
  link_with: [angelscript],
)

all_deps += angelscript
