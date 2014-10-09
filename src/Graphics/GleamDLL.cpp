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

#include <Shibboleth_IApp.h>

Shibboleth::ProxyAllocator g_Proxy_Allocator;

DYNAMICEXPORT bool InitGraphics(Shibboleth::IApp& app, const char* log_file_name)
{
#ifdef _UNICODE
	wchar_t buffer[256] = {0};
	mbstowcs(buffer, log_file_name, 256);
	Gleam::SetLogFileName(buffer);
#else
	Gleam::SetLogFileName(log_file_name);
#endif

	Shibboleth::SetAllocator(&app.getAllocator());
	g_Proxy_Allocator = Shibboleth::ProxyAllocator(Shibboleth::GetAllocator(), "Graphics Allocations");

	Gleam::SetAllocator(&g_Proxy_Allocator);
	return true;
}

DYNAMICEXPORT void ShutdownGraphics(void)
{
}

DYNAMICEXPORT Gleam::IShaderResourceView* CreateShaderResourceView(void)
{
	return Gleam::GetAllocator()->template allocT<Gleam::ShaderResourceView>();
}

DYNAMICEXPORT Gleam::IProgramBuffers* CreateProgramBuffers(void)
{
	return Gleam::GetAllocator()->template allocT<Gleam::ProgramBuffers>();
}

DYNAMICEXPORT Gleam::IRenderDevice* CreateRenderDevice(void)
{
	return Gleam::GetAllocator()->template allocT<Gleam::RenderDevice>();
}

DYNAMICEXPORT Gleam::IRenderTarget* CreateRenderTarget(void)
{
	return Gleam::GetAllocator()->template allocT<Gleam::RenderTarget>();
}

DYNAMICEXPORT Gleam::ISamplerState* CreateSamplerState(void)
{
	return Gleam::GetAllocator()->template allocT<Gleam::SamplerState>();
}

DYNAMICEXPORT Gleam::IRenderState* CreateRenderState(void)
{
	return Gleam::GetAllocator()->template allocT<Gleam::RenderState>();
}

DYNAMICEXPORT Gleam::ITexture* CreateTexture(void)
{
	return Gleam::GetAllocator()->template allocT<Gleam::Texture>();
}

DYNAMICEXPORT Gleam::ILayout* CreateLayout(void)
{
	return Gleam::GetAllocator()->template allocT<Gleam::Layout>();
}

DYNAMICEXPORT Gleam::IProgram* CreateProgram(void)
{
	return Gleam::GetAllocator()->template allocT<Gleam::Program>();
}

DYNAMICEXPORT Gleam::IShader* CreateShader(void)
{
	return Gleam::GetAllocator()->template allocT<Gleam::Shader>();
}

DYNAMICEXPORT Gleam::IBuffer* CreateBuffer(void)
{
	return Gleam::GetAllocator()->template allocT<Gleam::Buffer>();
}

DYNAMICEXPORT Gleam::IModel* CreateModel(void)
{
	return Gleam::GetAllocator()->template allocT<Gleam::Model>();
}

DYNAMICEXPORT Gleam::IMesh* CreateMesh(void)
{
	return Gleam::GetAllocator()->template allocT<Gleam::Mesh>();
}
