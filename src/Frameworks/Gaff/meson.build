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
  'include/Gaff_ArrayPtr.h',
  'include/Gaff_ArrayString.h',
  'include/Gaff_Assert.h',
  'include/Gaff_BitVector.h',
  'include/Gaff_ContainerAlgorithm.h',
  'include/Gaff_CrashHandler.h',
  'include/Gaff_Curve.h',
  'include/Gaff_DefaultAlignedAllocator.h',
  'include/Gaff_DefaultAllocator.h',
  'include/Gaff_Defer.h',
  'include/Gaff_Defines.h',
  'include/Gaff_DynamicLoader.h',
  'include/Gaff_DynamicModule.h',
  'include/Gaff_DynamicModule_Linux.h',
  'include/Gaff_DynamicModule_Windows.h',
  'include/Gaff_EnumIterator.h',
  'include/Gaff_Error.h',
  'include/Gaff_File.h',
  'include/Gaff_FileWatcher.h',
  'include/Gaff_FileWatcher_Windows.h',
  'include/Gaff_Flags.h',
  'include/Gaff_Function.h',
  'include/Gaff_GUID.h',
  'include/Gaff_Hash.h',
  'include/Gaff_HashString.h',
  'include/Gaff_Hashable.h',
  'include/Gaff_HashableFwd.h',
  'include/Gaff_IAllocator.h',
  'include/Gaff_IRefCounted.h',
  'include/Gaff_IncludeEASTLArray.h',
  'include/Gaff_IncludeEASTLAtomic.h',
  'include/Gaff_IncludeEASTLStringView.h',
  'include/Gaff_IncludeRapidJSON.h',
  'include/Gaff_IncludeTracy.h',
  'include/Gaff_IncludeWindows.h',
  'include/Gaff_JSON.h',
  'include/Gaff_JobPool.h',
  'include/Gaff_Math.h',
  'include/Gaff_MessagePack.h',
  'include/Gaff_NetworkDefines.h',
  'include/Gaff_Ops.h',
  'include/Gaff_Platform.h',
  'include/Gaff_Predicates.h',
  'include/Gaff_Queue.h',
  'include/Gaff_RefCounted.h',
  'include/Gaff_RefPtr.h',
  'include/Gaff_SmartPtrs.h',
  'include/Gaff_SparseStack.h',
  'include/Gaff_StackTrace.h',
  'include/Gaff_StackTrace_Linux.h',
  'include/Gaff_StackTrace_Windows.h',
  'include/Gaff_String.h',
  'include/Gaff_Utils.h',
  'include/Gaff_Vector.h',
  'include/Gaff_VectorMap.h',
]

source_files = [
  'src/Gaff_Assert.cpp',
  'src/Gaff_CrashHandler_Common.cpp',
  'src/Gaff_CrashHandler_Linux.cpp',
  'src/Gaff_CrashHandler_Windows.cpp',
  'src/Gaff_DynamicModule_Linux.cpp',
  'src/Gaff_DynamicModule_Windows.cpp',
  'src/Gaff_Error.cpp',
  'src/Gaff_File.cpp',
  'src/Gaff_FileWatcher_Windows.cpp',
  'src/Gaff_GUID.cpp',
  'src/Gaff_Hash.cpp',
  'src/Gaff_JSON.cpp',
  'src/Gaff_Math.cpp',
  'src/Gaff_MessagePack.cpp',
  'src/Gaff_StackTrace_Linux.cpp',
  'src/Gaff_StackTrace_Windows.cpp',
  'src/Gaff_Utils_Common.cpp',
  'src/Gaff_Utils_Linux.cpp',
  'src/Gaff_Utils_Mac.cpp',
  'src/Gaff_Utils_Windows.cpp',
]

extra_files = [
  'include/Gaff_Curve.inl',
  'include/Gaff_DynamicLoader.inl',
  'include/Gaff_FileWatcher.inl',
  'include/Gaff_Flags.inl',
  'include/Gaff_HashString.inl',
  'include/Gaff_JobPool.inl',
  'include/Gaff_SparseStack.inl',
  'include/Gaff_Utils_Common.inl',
]

include_dirs = [
  'include',
]

build_args = [
]

dependencies = [
  eastl_dep,
  rapidjson_dep,
  mpack_dep,
]

if is_msvc
  build_args += ['-D_CRT_SECURE_NO_WARNINGS']
endif

gaff = static_library(
  'Gaff',
  header_files,
  source_files,
  include_directories: include_dirs,
  cpp_args: default_cpp_args + build_args,
  extra_files: extra_files,
  dependencies: dependencies,
)

gaff_dep = declare_dependency(
  include_directories: include_dirs,
  link_with: [gaff],
  dependencies: dependencies,
)

all_frameworks += gaff
