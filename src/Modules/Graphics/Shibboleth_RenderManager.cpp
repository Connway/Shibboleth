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

#include "Shibboleth_RenderManager.h"
#include "Shibboleth_CameraComponent.h"

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

SHIB_REFLECTION_DEFINE(RenderManager, 0)

NS_SHIBBOLETH

SHIB_REFLECTION_CLASS_DEFINE_BEGIN(RenderManager)
	.BASE(IRenderManager)
	.BASE(IManager)
	.ctor<>()

	//.func("createShaderResourceView", &RenderManager::createShaderResourceView)
	//.func("createDepthStencilState", &RenderManager::createDepthStencilState)
	//.func("createRenderDevice", &RenderManager::createRenderDevice)
	//.func("createRenderOutput", &RenderManager::createRenderOutput)
	//.func("createRenderTarget", &RenderManager::createRenderTarget)
	//.func("createSamplerState", &RenderManager::createSamplerState)
	//.func("createCommandList", &RenderManager::createCommandList)
	//.func("createRasterState", &RenderManager::createRasterState)
	//.func("createBlendState", &RenderManager::createBlendState)
	//.func("createTexture", &RenderManager::createTexture)
	//.func("createProgramBuffers", &RenderManager::createProgramBuffers)
	//.func("createProgram", &RenderManager::createProgram)
	//.func("createShader", &RenderManager::createShader)
	//.func("createBuffer", &RenderManager::createBuffer)
	//.func("createLayout", &RenderManager::createLayout)
	//.func("createModel", &RenderManager::createModel)
	//.func("createMesh", &RenderManager::createMesh)

	.func("setActiveCamera", &RenderManager::setActiveCamera)
SHIB_REFLECTION_CLASS_DEFINE_END(RenderManager)

Gleam::IShaderResourceView* RenderManager::createShaderResourceView(void) const
{
	return SHIB_ALLOCT(Gleam::ShaderResourceView, *GetAllocator());
}

Gleam::IDepthStencilState* RenderManager::createDepthStencilState(void) const
{
	return SHIB_ALLOCT(Gleam::DepthStencilState, *GetAllocator());
}

Gleam::IRenderDevice* RenderManager::createRenderDevice(void) const
{
	return SHIB_ALLOCT(Gleam::RenderDevice, *GetAllocator());
}

Gleam::IRenderOutput* RenderManager::createRenderOutput(void) const
{
	return SHIB_ALLOCT(Gleam::RenderOutput, *GetAllocator());
}

Gleam::IRenderTarget* RenderManager::createRenderTarget(void) const
{
	return SHIB_ALLOCT(Gleam::RenderTarget, *GetAllocator());
}

Gleam::ISamplerState* RenderManager::createSamplerState(void) const
{
	return SHIB_ALLOCT(Gleam::SamplerState, *GetAllocator());
}

Gleam::ICommandList* RenderManager::createCommandList(void) const
{
	return SHIB_ALLOCT(Gleam::CommandList, *GetAllocator());
}

Gleam::IRasterState* RenderManager::createRasterState(void) const
{
	return SHIB_ALLOCT(Gleam::RasterState, *GetAllocator());
}

Gleam::IBlendState* RenderManager::createBlendState(void) const
{
	return SHIB_ALLOCT(Gleam::BlendState, *GetAllocator());
}

Gleam::ITexture* RenderManager::createTexture(void) const
{
	return SHIB_ALLOCT(Gleam::Texture, *GetAllocator());
}

Gleam::IProgramBuffers* RenderManager::createProgramBuffers(void) const
{
	return SHIB_ALLOCT(Gleam::ProgramBuffers, *GetAllocator());
}

Gleam::IProgram* RenderManager::createProgram(void) const
{
	return SHIB_ALLOCT(Gleam::Program, *GetAllocator());
}

Gleam::IShader* RenderManager::createShader(void) const
{
	return SHIB_ALLOCT(Gleam::Shader, *GetAllocator());
}

Gleam::IBuffer* RenderManager::createBuffer(void) const
{
	return SHIB_ALLOCT(Gleam::Buffer, *GetAllocator());
}

Gleam::ILayout* RenderManager::createLayout(void) const
{
	return SHIB_ALLOCT(Gleam::Layout, *GetAllocator());
}

Gleam::IModel* RenderManager::createModel(void) const
{
	return SHIB_ALLOCT(Gleam::Model, *GetAllocator());
}

Gleam::IMesh* RenderManager::createMesh(void) const
{
	return SHIB_ALLOCT(Gleam::Mesh, *GetAllocator());
}

void RenderManager::setActiveCamera(CameraComponent* camera)
{
	GAFF_REF(camera);
}

NS_END
