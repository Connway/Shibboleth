/************************************************************************************
Copyright (C) 2018 by Nicholas LaCroix

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
************************************************************************************/

#include "Shibboleth_GraphicsManager.h"
#include <Gleam_ShaderResourceView.h>
#include <Gleam_DepthStencilState.h>
#include <Gleam_RenderDevice.h>
#include <Gleam_RenderOutput.h>
#include <Gleam_RenderTarget.h>
#include <Gleam_SamplerState.h>
#include <Gleam_CommandList.h>
#include <Gleam_RasterState.h>
#include <Gleam_BlendState.h>
#include <Gleam_Texture.h>
#include <Gleam_Program.h>
#include <Gleam_Shader.h>
#include <Gleam_Buffer.h>
#include <Gleam_Layout.h>
#include <Gleam_Model.h>
#include <Gleam_Mesh.h>
#include <Gleam_IncludeD3D11.h>

SHIB_REFLECTION_DEFINE(GraphicsManager, 0)

NS_SHIBBOLETH

SHIB_REFLECTION_CLASS_DEFINE_BEGIN(GraphicsManager)
	.BASE(IGraphicsManager)
	.BASE(IManager)
	.ctor<>()
SHIB_REFLECTION_CLASS_DEFINE_END(GraphicsManager)

Gleam::IShaderResourceView* GraphicsManager::createShaderResourceView(void) const
{
	return SHIB_ALLOCT(Gleam::ShaderResourceView, *GetAllocator());
}

Gleam::IDepthStencilState* GraphicsManager::createDepthStencilState(void) const
{
	return SHIB_ALLOCT(Gleam::DepthStencilState, *GetAllocator());
}

Gleam::IRenderDevice* GraphicsManager::createRenderDevice(void) const
{
	return SHIB_ALLOCT(Gleam::RenderDevice, *GetAllocator());
}

Gleam::IRenderOutput* GraphicsManager::createRenderOutput(void) const
{
	return SHIB_ALLOCT(Gleam::RenderOutput, *GetAllocator());
}

Gleam::IRenderTarget* GraphicsManager::createRenderTarget(void) const
{
	return SHIB_ALLOCT(Gleam::RenderTarget, *GetAllocator());
}

Gleam::ISamplerState* GraphicsManager::createSamplerState(void) const
{
	return SHIB_ALLOCT(Gleam::SamplerState, *GetAllocator());
}

Gleam::ICommandList* GraphicsManager::createCommandList(void) const
{
	return SHIB_ALLOCT(Gleam::CommandList, *GetAllocator());
}

Gleam::IRasterState* GraphicsManager::createRasterState(void) const
{
	return SHIB_ALLOCT(Gleam::RasterState, *GetAllocator());
}

Gleam::IBlendState* GraphicsManager::createBlendState(void) const
{
	return SHIB_ALLOCT(Gleam::BlendState, *GetAllocator());
}

Gleam::ITexture* GraphicsManager::createTexture(void) const
{
	return SHIB_ALLOCT(Gleam::Texture, *GetAllocator());
}

Gleam::IProgramBuffers* GraphicsManager::createProgramBuffers(void) const
{
	return SHIB_ALLOCT(Gleam::ProgramBuffers, *GetAllocator());
}

Gleam::IProgram* GraphicsManager::createProgram(void) const
{
	return SHIB_ALLOCT(Gleam::Program, *GetAllocator());
}

Gleam::IShader* GraphicsManager::createShader(void) const
{
	return SHIB_ALLOCT(Gleam::Shader, *GetAllocator());
}

Gleam::IBuffer* GraphicsManager::createBuffer(void) const
{
	return SHIB_ALLOCT(Gleam::Buffer, *GetAllocator());
}

Gleam::ILayout* GraphicsManager::createLayout(void) const
{
	return SHIB_ALLOCT(Gleam::Layout, *GetAllocator());
}

Gleam::IModel* GraphicsManager::createModel(void) const
{
	return SHIB_ALLOCT(Gleam::Model, *GetAllocator());
}

Gleam::IMesh* GraphicsManager::createMesh(void) const
{
	return SHIB_ALLOCT(Gleam::Mesh, *GetAllocator());
}

NS_END
