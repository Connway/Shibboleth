header_files = [
  'include/Esprit_StateMachine.h',
  'include/Esprit_HashString.h',
  'include/Esprit_String.h',
  'include/Esprit_IProcess.h',
  'include/Esprit_Skeleton.h',
  'include/Esprit_Vector.h',
  'include/Esprit_VariableSet.h',
  'include/Esprit_ProxyAllocator.h',
  'include/Esprit_Defines.h',
  'include/Esprit_Global.h',
  'include/Esprit_Pose.h',
  'include/Esprit_BitVector.h',
  'include/Esprit_VectorMap.h',
  'include/Esprit_ICondition.h',
  'include/Esprit_SmartPtrs.h',
  'include/Esprit_StateMachineCommonConditions.h',
]

source_files = [
  'src/Esprit_VariableSet.cpp',
  'src/Esprit_Pose.cpp',
  'src/Esprit_Global.cpp',
  'src/Esprit_StateMachine.cpp',
  'src/Esprit_Skeleton.cpp',
  'src/Esprit_StateMachineCommonConditions.cpp',
]

include_dirs = [
  'include',
  '../Gaff/include',
  '../Gleam/include',
  '../../Dependencies/EASTL/include',
  '../../Dependencies/glfw/include',
  '../../Dependencies/glm',
]

build_args = [
]

esprit = static_library(
  'Esprit',
  header_files,
  source_files,
  include_directories: include_dirs,
  cpp_args: default_cpp_args + build_args,
  # pic: true,
)

