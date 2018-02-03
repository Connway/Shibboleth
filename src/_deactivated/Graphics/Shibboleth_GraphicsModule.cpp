/************************************************************************************
Copyright (C) 2017 by Nicholas LaCroix

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

#include <Shibboleth_ProxyAllocator.h>
#include <Shibboleth_Utilities.h>
#include <Shibboleth_String.h>
#include <Shibboleth_IApp.h>
#include <Gleam_ShaderResourceView.h>
#include <Gleam_DepthStencilState.h>
#include <Gleam_RenderDevice.h>
#include <Gleam_RenderTarget.h>
#include <Gleam_SamplerState.h>
#include <Gleam_RasterState.h>
#include <Gleam_CommandList.h>
#include <Gleam_BlendState.h>
#include <Gleam_Texture.h>
#include <Gleam_Program.h>
#include <Gleam_Window.h>
#include <Gleam_Layout.h>
#include <Gleam_Shader.h>
#include <Gleam_Buffer.h>
#include <Gleam_Model.h>
#include <Gleam_Mesh.h>

#include <Gleam_Keyboard.h>
#include <Gleam_Mouse.h>

static Shibboleth::ProxyAllocator g_proxy_allocator("Graphics");
static Shibboleth::U8String g_graphics_log_file;

static void GraphicsLog(const char* message, Gleam::LOG_MSG_TYPE type)
{
	static Shibboleth::LogManager& lm = Shibboleth::GetApp().getLogManager();

	lm.logMessage(
		static_cast<Shibboleth::LogManager::LOG_TYPE>(type),
		g_graphics_log_file.getBuffer(),
		message
	);
}


DYNAMICEXPORT_C bool InitGraphics(Shibboleth::IApp& app, const char* log_file_name)
{
	g_graphics_log_file = log_file_name;
	Gleam::SetAllocator(&g_proxy_allocator);
	Gleam::SetLogFunc(GraphicsLog);
	Shibboleth::SetApp(app);
	return true;
}

DYNAMICEXPORT_C void ShutdownGraphics(void)
{
}

DYNAMICEXPORT_C Gleam::IWindow* CreateWindowS(void)
{
	return SHIB_ALLOCT(Gleam::Window, g_proxy_allocator);
}

DYNAMICEXPORT_C void UpdateWindows(void)
{
	Gleam::Window::HandleWindowMessages();
}

DYNAMICEXPORT_C Gleam::IShaderResourceView* CreateShaderResourceView(void)
{
	return SHIB_ALLOCT(Gleam::ShaderResourceView, g_proxy_allocator);
}

DYNAMICEXPORT_C Gleam::IDepthStencilState* CreateDepthStencilState(void)
{
	return SHIB_ALLOCT(Gleam::DepthStencilState, g_proxy_allocator);
}

DYNAMICEXPORT_C Gleam::IProgramBuffers* CreateProgramBuffers(void)
{
	return SHIB_ALLOCT(Gleam::ProgramBuffers, g_proxy_allocator);
}

DYNAMICEXPORT_C Gleam::IRenderDevice* CreateRenderDevice(void)
{
	return SHIB_ALLOCT(Gleam::RenderDevice, g_proxy_allocator);
}

DYNAMICEXPORT_C Gleam::IRenderTarget* CreateRenderTarget(void)
{
	return SHIB_ALLOCT(Gleam::RenderTarget, g_proxy_allocator);
}

DYNAMICEXPORT_C Gleam::ISamplerState* CreateSamplerState(void)
{
	return SHIB_ALLOCT(Gleam::SamplerState, g_proxy_allocator);
}

DYNAMICEXPORT_C Gleam::IRasterState* CreateRasterState(void)
{
	return SHIB_ALLOCT(Gleam::RasterState, g_proxy_allocator);
}

DYNAMICEXPORT_C Gleam::ICommandList* CreateCommandList(void)
{
	return SHIB_ALLOCT(Gleam::CommandList, g_proxy_allocator);
}

DYNAMICEXPORT_C Gleam::IBlendState* CreateBlendState(void)
{
	return SHIB_ALLOCT(Gleam::BlendState, g_proxy_allocator);
}

DYNAMICEXPORT_C Gleam::ITexture* CreateTexture(void)
{
	return SHIB_ALLOCT(Gleam::Texture, g_proxy_allocator);
}

DYNAMICEXPORT_C Gleam::IProgram* CreateProgram(void)
{
	return SHIB_ALLOCT(Gleam::Program, g_proxy_allocator);
}

DYNAMICEXPORT_C Gleam::ILayout* CreateLayout(void)
{
	return SHIB_ALLOCT(Gleam::Layout, g_proxy_allocator);
}

DYNAMICEXPORT_C Gleam::IShader* CreateShader(void)
{
	return SHIB_ALLOCT(Gleam::Shader, g_proxy_allocator);
}

DYNAMICEXPORT_C Gleam::IBuffer* CreateBuffer(void)
{
	return SHIB_ALLOCT(Gleam::Buffer, g_proxy_allocator);
}

DYNAMICEXPORT_C Gleam::IModel* CreateModel(void)
{
	return SHIB_ALLOCT(Gleam::Model, g_proxy_allocator);
}

DYNAMICEXPORT_C Gleam::IMesh* CreateMesh(void)
{
	return SHIB_ALLOCT(Gleam::Mesh, g_proxy_allocator);
}

DYNAMICEXPORT_C Gleam::IKeyboard* CreateKeyboard(void)
{
	return SHIB_ALLOCT(Gleam::Keyboard, g_proxy_allocator);
}

DYNAMICEXPORT_C Gleam::IMouse* CreateMouse(void)
{
	return SHIB_ALLOCT(Gleam::Mouse, g_proxy_allocator);
}

DYNAMICEXPORT_C const char* GetShaderExtension(void)
{
#ifdef USE_DX
	return ".hlsl";
#else
	return ".glsl";
#endif
}
