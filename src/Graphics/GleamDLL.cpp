/************************************************************************************
Copyright (C) 2014 by Nicholas LaCroix

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

#include <Shibboleth_Memory.h>
#include <Gleam_ShaderResourceView.h>
#include <Gleam_RenderDevice.h>
#include <Gleam_RenderTarget.h>
#include <Gleam_SamplerState.h>
#include <Gleam_RenderState.h>
#include <Gleam_Texture.h>
#include <Gleam_Program.h>
#include <Gleam_Layout.h>
#include <Gleam_Shader.h>
#include <Gleam_Buffer.h>
#include <Gleam_Model.h>
#include <Gleam_Mesh.h>

#include <Gleam_Window.h>
#include <Shibboleth_IApp.h>

static Shibboleth::ProxyAllocator g_Proxy_Allocator;

DYNAMICEXPORT_C bool InitGraphics(Shibboleth::IApp&, const char* log_file_name)
{
#ifdef _UNICODE
	wchar_t buffer[256] = {0};
	mbstowcs(buffer, log_file_name, 256);
	Gleam::SetLogFileName(buffer);
#else
	Gleam::SetLogFileName(log_file_name);
#endif

	g_Proxy_Allocator = Shibboleth::ProxyAllocator(Shibboleth::GetAllocator(), "Graphics Allocations");

	Gleam::SetAllocator(&g_Proxy_Allocator);
	return true;
}

DYNAMICEXPORT_C void ShutdownGraphics(void)
{
	Gleam::Window::clear();
}

DYNAMICEXPORT_C Gleam::IWindow* CreateWindowS(void)
{
	return g_Proxy_Allocator.template allocT<Gleam::Window>();
}

DYNAMICEXPORT_C void DestroyWindowS(Gleam::IWindow* window)
{
	g_Proxy_Allocator.freeT(window);
}

DYNAMICEXPORT_C void UpdateWindows(void)
{
	Gleam::Window::handleWindowMessages();
}

DYNAMICEXPORT_C Gleam::IShaderResourceView* CreateShaderResourceView(void)
{
	return g_Proxy_Allocator.template allocT<Gleam::ShaderResourceView>();
}

DYNAMICEXPORT_C Gleam::IProgramBuffers* CreateProgramBuffers(void)
{
	return g_Proxy_Allocator.template allocT<Gleam::ProgramBuffers>();
}

DYNAMICEXPORT_C Gleam::IRenderDevice* CreateRenderDevice(void)
{
	return g_Proxy_Allocator.template allocT<Gleam::RenderDevice>();
}

DYNAMICEXPORT_C Gleam::IRenderTarget* CreateRenderTarget(void)
{
	return g_Proxy_Allocator.template allocT<Gleam::RenderTarget>();
}

DYNAMICEXPORT_C Gleam::ISamplerState* CreateSamplerState(void)
{
	return g_Proxy_Allocator.template allocT<Gleam::SamplerState>();
}

DYNAMICEXPORT_C Gleam::IRenderState* CreateRenderState(void)
{
	return g_Proxy_Allocator.template allocT<Gleam::RenderState>();
}

DYNAMICEXPORT_C Gleam::ITexture* CreateTexture(void)
{
	return g_Proxy_Allocator.template allocT<Gleam::Texture>();
}

DYNAMICEXPORT_C Gleam::ILayout* CreateLayout(void)
{
	return g_Proxy_Allocator.template allocT<Gleam::Layout>();
}

DYNAMICEXPORT_C Gleam::IProgram* CreateProgram(void)
{
	return g_Proxy_Allocator.template allocT<Gleam::Program>();
}

DYNAMICEXPORT_C Gleam::IShader* CreateShader(void)
{
	return g_Proxy_Allocator.template allocT<Gleam::Shader>();
}

DYNAMICEXPORT_C Gleam::IBuffer* CreateBuffer(void)
{
	return g_Proxy_Allocator.template allocT<Gleam::Buffer>();
}

DYNAMICEXPORT_C Gleam::IModel* CreateModel(void)
{
	return g_Proxy_Allocator.template allocT<Gleam::Model>();
}

DYNAMICEXPORT_C Gleam::IMesh* CreateMesh(void)
{
	return g_Proxy_Allocator.template allocT<Gleam::Mesh>();
}

DYNAMICEXPORT_C const char* GetShaderExtension(void)
{
#ifdef USE_DX
	return ".hlsl";
#else
	return ".glsl";
#endif
}
