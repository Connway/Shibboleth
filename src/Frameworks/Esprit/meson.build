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
  'include/Esprit_BitVector.h',
  'include/Esprit_Defines.h',
  'include/Esprit_Global.h',
  'include/Esprit_HashString.h',
  'include/Esprit_ICondition.h',
  'include/Esprit_IProcess.h',
  'include/Esprit_Pose.h',
  'include/Esprit_ProxyAllocator.h',
  'include/Esprit_Skeleton.h',
  'include/Esprit_SmartPtrs.h',
  'include/Esprit_StateMachine.h',
  'include/Esprit_StateMachineCommonConditions.h',
  'include/Esprit_String.h',
  'include/Esprit_VariableSet.h',
  'include/Esprit_Vector.h',
  'include/Esprit_VectorMap.h',
]

source_files = [
  'src/Esprit_Global.cpp',
  'src/Esprit_Pose.cpp',
  'src/Esprit_Skeleton.cpp',
  'src/Esprit_StateMachine.cpp',
  'src/Esprit_StateMachineCommonConditions.cpp',
  'src/Esprit_VariableSet.cpp',
]

include_dirs = [
  'include',
]

build_args = [
]

dependencies = [
  gaff_dep,
  gleam_dep,
]

esprit = static_library(
  'Esprit',
  header_files,
  source_files,
  include_directories: include_dirs,
  cpp_args: default_cpp_args + build_args,
  dependencies: dependencies,
)

esprit_dep = declare_dependency(
  include_directories: ['include'],
  link_with: [esprit],
  dependencies: dependencies,
)

all_frameworks += esprit
