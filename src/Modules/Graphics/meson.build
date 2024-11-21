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
  'include/Camera/Shibboleth_CameraPipelineData.h',
  'include/Camera/Shibboleth_CameraView.h',
  'include/Model/Shibboleth_MeshResource.h',
  'include/Model/Shibboleth_ModelComponent.h',
  'include/Model/Shibboleth_ModelInstanceData.h',
  'include/Model/Shibboleth_ModelPipelineData.h',
  'include/Model/Shibboleth_ModelResource.h',
  'include/Pipelines/Shibboleth_IRenderPipelineData.h',
  'include/Pipelines/Shibboleth_IRenderPipelineStage.h',
  'include/Pipelines/Shibboleth_RenderPipeline.h',
  'include/Pipelines/Shibboleth_RenderPipelineSystem.h',
  'include/Pipelines/Stages/Shibboleth_ClearRenderTargetStage.h',
  'include/Pipelines/Stages/Shibboleth_ProcessModelsStage.h',
  'include/Pipelines/Stages/Shibboleth_RenderCommandStage.h',
  'include/Pipelines/Stages/Shibboleth_RenderGBufferStage.h',
  'include/Resources/Shibboleth_BufferResource.h',
  'include/Resources/Shibboleth_ImageResource.h',
  'include/Resources/Shibboleth_MaterialResource.h',
  'include/Resources/Shibboleth_ProgramBuffersResource.h',
  'include/Resources/Shibboleth_RasterStateResource.h',
  'include/Resources/Shibboleth_RenderTargetResource.h',
  'include/Resources/Shibboleth_SamplerStateResource.h',
  'include/Resources/Shibboleth_ShaderResource.h',
  'include/Resources/Shibboleth_TextureResource.h',
  'include/Shibboleth_GraphicsConfig.h',
  'include/Shibboleth_GraphicsDefines.h',
  'include/Shibboleth_GraphicsLogging.h',
  'include/Shibboleth_GraphicsReflection.h',
  'include/Shibboleth_RenderCommands.h',
  'include/Shibboleth_RenderManager.h',
]

source_files = [
  'src/Camera/Shibboleth_CameraPipelineData.cpp',
  'src/Camera/Shibboleth_CameraView.cpp',
  'src/Model/Shibboleth_MeshResource.cpp',
  'src/Model/Shibboleth_ModelComponent.cpp',
  'src/Model/Shibboleth_ModelInstanceData.cpp',
  'src/Model/Shibboleth_ModelPipelineData.cpp',
  'src/Model/Shibboleth_ModelResource.cpp',
  'src/Pipelines/Shibboleth_IRenderPipelineData.cpp',
  'src/Pipelines/Shibboleth_IRenderPipelineStage.cpp',
  'src/Pipelines/Shibboleth_RenderPipeline.cpp',
  'src/Pipelines/Shibboleth_RenderPipelineSystem.cpp',
  'src/Pipelines/Stages/Shibboleth_ClearRenderTargetStage.cpp',
  'src/Pipelines/Stages/Shibboleth_ProcessModelsStage.cpp',
  'src/Pipelines/Stages/Shibboleth_RenderCommandStage.cpp',
  'src/Pipelines/Stages/Shibboleth_RenderGBufferStage.cpp',
  'src/Resources/Shibboleth_BufferResource.cpp',
  'src/Resources/Shibboleth_ImageResource.cpp',
  'src/Resources/Shibboleth_MaterialResource.cpp',
  'src/Resources/Shibboleth_ProgramBuffersResource.cpp',
  'src/Resources/Shibboleth_RasterStateResource.cpp',
  'src/Resources/Shibboleth_RenderTargetResource.cpp',
  'src/Resources/Shibboleth_SamplerStateResource.cpp',
  'src/Resources/Shibboleth_ShaderResource.cpp',
  'src/Resources/Shibboleth_TextureResource.cpp',
  'src/Shibboleth_GraphicsConfig.cpp',
  'src/Shibboleth_GraphicsModule.cpp',
  'src/Shibboleth_GraphicsReflection.cpp',
  'src/Shibboleth_RenderManager.cpp',
]

module_source_files = [
  'src/Shibboleth_GraphicsModule.cpp',
]

generated_files = [
  custom_target(
    'Gen_ReflectionInit.h',
    output: 'Gen_ReflectionInit.h',
    command: [code_generator, 'module_header', '-rp', '@SOURCE_ROOT@', '-of', '@OUTPUT@', '-m', 'Graphics'],
    depends: [code_generator],
    build_always_stale: true,
  ),
]

extra_files = [
]

include_dirs = [
  'include',
]

build_args = [
  '-DSHIB_STATIC',
]

module_build_args = [
]

dependencies = [
  minizip_ng_dep,
  assimp_dep,
  zstd_dep,

  gleam_dep,

  engine_dep,
  memory_dep,

  main_loop_dep,
  resource_dep,
  entity_dep,
]

graphics = static_library(
  'Graphics',
  header_files,
  source_files,
  generated_files,
  include_directories: include_dirs,
  extra_files: extra_files,
  cpp_args: default_cpp_args + build_args,
  dependencies: default_dependencies + default_module_dependencies + dependencies,
)

graphics_dep = declare_dependency(
  include_directories: include_dirs,
  dependencies: dependencies,
  link_with: [graphics],
  sources: [generated_files],
)

graphics_module = shared_library(
  'GraphicsModule',
  module_source_files,
  cpp_args: default_cpp_args + module_build_args,
  dependencies: graphics_dep,
  install: true,
  install_dir: 'Modules',
)

all_modules += [graphics, graphics_module]

