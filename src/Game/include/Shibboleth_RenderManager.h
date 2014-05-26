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

#pragma once

#include "Shibboleth_App.h"
#include <Gleam_Window.h>

namespace Gaff
{
	class JSON;
}

namespace Gleam
{
	class IShaderResourceView;
	class IRenderDevice;
	class IRenderTarget;
	class ISamplerState;
	class IRenderState;
	class ITexture;
	class ILayout;
	class IProgram;
	class IShader;
	class IBuffer;
	class IModel;
	class IMesh;
}

NS_SHIBBOLETH

class RenderManager : public Gaff::INamedObject
{
public:
	RenderManager(App& app);
	~RenderManager(void);

	const char* getName(void) const;

	bool init(const char* cfg_file);

	Gleam::IRenderDevice& getRenderDevice(void);
	Gaff::SpinLock& getSpinLock(void);

	void printfLoadLog(const char* format, ...);

	//void addRenderPacket();

	INLINE Gleam::IShaderResourceView* createShaderResourceView(void);
	INLINE Gleam::IRenderDevice* createRenderDevice(void);
	INLINE Gleam::IRenderTarget* createRenderTarget(void);
	INLINE Gleam::ISamplerState* createSamplerState(void);
	INLINE Gleam::IRenderState* createRenderState(void);
	INLINE Gleam::ITexture* createTexture(void);
	INLINE Gleam::ILayout* createLayout(void);
	INLINE Gleam::IProgram* createProgram(void);
	INLINE Gleam::IShader* createShader(void);
	INLINE Gleam::IBuffer* createBuffer(void);
	INLINE Gleam::IModel* createModel(void);
	INLINE Gleam::IMesh* createMesh(void);

private:
	struct GraphicsFunctions
	{
		typedef bool (*InitGraphics)(App& app, const char* log_file_name);
		typedef void (*ShutdownGraphics)(void);

		typedef Gleam::IShaderResourceView* (*CreateShaderResourceView)(void);
		typedef Gleam::IRenderDevice* (*CreateRenderDevice)(void);
		typedef Gleam::IRenderTarget* (*CreateRenderTarget)(void);
		typedef Gleam::ISamplerState* (*CreateSamplerState)(void);
		typedef Gleam::IRenderState* (*CreateRenderState)(void);
		typedef Gleam::ITexture* (*CreateTexture)(void);
		typedef Gleam::ILayout* (*CreateLayout)(void);
		typedef Gleam::IProgram* (*CreateProgram)(void);
		typedef Gleam::IShader* (*CreateShader)(void);
		typedef Gleam::IBuffer* (*CreateBuffer)(void);
		typedef Gleam::IModel* (*CreateModel)(void);
		typedef Gleam::IMesh* (*CreateMesh)(void);

		ShutdownGraphics shutdown;
		CreateShaderResourceView create_shaderresourceview;
		CreateRenderDevice create_renderdevice;
		CreateRenderTarget create_rendertarget;
		CreateSamplerState create_samplerstate;
		CreateRenderState create_renderstate;
		CreateTexture create_texture;
		CreateLayout create_layout;
		CreateProgram create_program;
		CreateShader create_shader;
		CreateBuffer create_buffer;
		CreateModel create_model;
		CreateMesh create_mesh;
	};

	//struct RenderPacket
	//{
	//};

	GraphicsFunctions _graphics_functions;
	Array< Gaff::SmartPtr<Gleam::Window> > _windows;
	Gaff::SmartPtr<Gleam::IRenderDevice, ProxyAllocator> _render_device;
	DynamicLoader::ModulePtr _gleam_module;
	Gaff::SpinLock _spin_lock;

	App& _app;

	void generateDefaultConfig(Gaff::JSON& cfg);
	bool cacheGleamFunctions(App& app, const Gaff::JSON& module, const char* cfg_file);
};

NS_END
