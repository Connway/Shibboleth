/************************************************************************************
Copyright (C) 2022 by Nicholas LaCroix

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
#include <Gleam_Window.h>
#include <Gleam_Mesh.h>
#include <Gaff_Function.h>

SHIB_REFLECTION_DEFINE_BEGIN(Shibboleth::RenderManager)
	.template BASE(Shibboleth::RenderManagerBase)
	.template BASE(Shibboleth::IRenderManager)
	.template base<Shibboleth::IManager>()
	.template ctor<>()
SHIB_REFLECTION_DEFINE_END(Shibboleth::RenderManager)


NS_SHIBBOLETH

SHIB_REFLECTION_CLASS_DEFINE(RenderManager)

static ProxyAllocator g_allocator("Graphics");

static consteval Gleam::RendererType GetRendererType(void)
{
#ifdef USE_D3D11
	return Gleam::RendererType::Direct3D11;
#elif defined(USE_D3D12)
	return Gleam::RendererType::DIRECT3D12;
#elif defined(USE_VULKAN)
	return Gleam::RendererType::VULKAN;
#endif
}

RenderManager::RenderManager(void)
{
}

RenderManager::~RenderManager(void)
{
}

Gleam::RendererType RenderManager::getRendererType(void) const
{
	return GetRendererType();
}

Gleam::IShaderResourceView* RenderManager::createShaderResourceView(void) const
{
	return SHIB_ALLOCT(Gleam::ShaderResourceView, g_allocator);
}

Gleam::IDepthStencilState* RenderManager::createDepthStencilState(void) const
{
	return SHIB_ALLOCT(Gleam::DepthStencilState, g_allocator);
}

Gleam::IRenderDevice* RenderManager::createRenderDevice(void) const
{
	return SHIB_ALLOCT(Gleam::RenderDevice, g_allocator);
}

Gleam::IRenderOutput* RenderManager::createRenderOutput(void) const
{
	return SHIB_ALLOCT(Gleam::RenderOutput, g_allocator);
}

Gleam::IRenderTarget* RenderManager::createRenderTarget(void) const
{
	return SHIB_ALLOCT(Gleam::RenderTarget, g_allocator);
}

Gleam::ISamplerState* RenderManager::createSamplerState(void) const
{
	return SHIB_ALLOCT(Gleam::SamplerState, g_allocator);
}

Gleam::ICommandList* RenderManager::createCommandList(void) const
{
	return SHIB_ALLOCT(Gleam::CommandList, g_allocator);
}

Gleam::IRasterState* RenderManager::createRasterState(void) const
{
	return SHIB_ALLOCT(Gleam::RasterState, g_allocator);
}

Gleam::IBlendState* RenderManager::createBlendState(void) const
{
	return SHIB_ALLOCT(Gleam::BlendState, g_allocator);
}

Gleam::ITexture* RenderManager::createTexture(void) const
{
	return SHIB_ALLOCT(Gleam::Texture, g_allocator);
}

Gleam::IProgramBuffers* RenderManager::createProgramBuffers(void) const
{
	return SHIB_ALLOCT(Gleam::ProgramBuffers, g_allocator);
}

Gleam::IProgram* RenderManager::createProgram(void) const
{
	return SHIB_ALLOCT(Gleam::Program, g_allocator);
}

Gleam::IShader* RenderManager::createShader(void) const
{
	return SHIB_ALLOCT(Gleam::Shader, g_allocator);
}

Gleam::IBuffer* RenderManager::createBuffer(void) const
{
	return SHIB_ALLOCT(Gleam::Buffer, g_allocator);
}

Gleam::ILayout* RenderManager::createLayout(void) const
{
	return SHIB_ALLOCT(Gleam::Layout, g_allocator);
}

Gleam::IMesh* RenderManager::createMesh(void) const
{
	return SHIB_ALLOCT(Gleam::Mesh, g_allocator);
}

Gleam::IRenderDevice::AdapterList RenderManager::getDisplayModes(void) const
{
	return Gleam::GetDisplayModes<GetRendererType()>();
}

Gleam::Window* RenderManager::createWindow(void) const
{
	return SHIB_ALLOCT(Gleam::Window, g_allocator);
}

void RenderManager::updateWindows(void)
{
	Gleam::Window::PollEvents();
}

NS_END
