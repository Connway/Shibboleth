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

#include <Shibboleth_ReflectionDefinitions.h>
#include <Shibboleth_IManager.h>
#include <Shibboleth_App.h>
#include <Gleam_IWindow.h>

namespace Gaff
{
	class JSON;
}

namespace Gleam
{
	class IShaderResourceView;
	class IProgramBuffers;
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

class RenderManager : public IManager
{
public:
	struct WindowData
	{
		Gleam::IWindow* window;
		unsigned int device;
		unsigned int output;
	};


	RenderManager(IApp& app);
	~RenderManager(void);

	const char* getName(void) const;

	bool init(const char* cfg_file);

	Gleam::IRenderDevice& getRenderDevice(void);
	Gaff::SpinLock& getSpinLock(void);

	void printfLoadLog(const char* format, ...);

	// Don't call this in a thread sensitive environment
	bool createWindow(
		const wchar_t* app_name, Gleam::IWindow::MODE window_mode,
		int x, int y, unsigned int width, unsigned int height,
		unsigned int refresh_rate, const char* device_name,
		unsigned int adapter_id, unsigned int display_id, bool vsync
	);

	INLINE void updateWindows(void);

	INLINE unsigned int getNumWindows(void) const;
	INLINE const WindowData& getWindowData(unsigned int window) const;

	INLINE const char* getShaderExtension(void) const;
	INLINE Gleam::IShaderResourceView* createShaderResourceView(void);
	INLINE Gleam::IProgramBuffers* createProgramBuffers(void);
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

	void* rawRequestInterface(unsigned int class_id) const;

	static void InitReflectionDefinition(void);

private:
	struct GraphicsFunctions
	{
		typedef bool (*InitGraphics)(IApp& app, const char* log_file_name);
		typedef void (*ShutdownGraphics)(void);
		typedef const char* (*GetShaderExtension)(void);
		typedef void (*UpdateWindows)(void);
		typedef Gleam::IWindow* (*CreateWindow)(void);
		typedef void (*DestroyWindow)(Gleam::IWindow*);

		typedef Gleam::IShaderResourceView* (*CreateShaderResourceView)(void);
		typedef Gleam::IProgramBuffers* (*CreateProgramBuffers)(void);
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

		UpdateWindows update_windows;
		CreateWindow create_window;
		DestroyWindow destroy_window;

		GetShaderExtension get_shader_extension;
		CreateShaderResourceView create_shaderresourceview;
		CreateProgramBuffers create_programbuffers;
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

	GraphicsFunctions _graphics_functions;
	Array<WindowData> _windows;
	Gaff::SmartPtr<Gleam::IRenderDevice, ProxyAllocator> _render_device;
	DynamicLoader::ModulePtr _gleam_module;

	Gaff::SpinLock _rd_lock;

	ProxyAllocator _proxy_allocator;
	IApp& _app;

	int getDisplayModeID(unsigned int width, unsigned int height, unsigned int refresh_rate, unsigned int adapter_id, unsigned int display_id);
	void generateDefaultConfig(Gaff::JSON& cfg);
	bool cacheGleamFunctions(IApp& app, const Gaff::JSON& module, const char* cfg_file);

	GAFF_NO_COPY(RenderManager);
	GAFF_NO_MOVE(RenderManager);

	REF_DEF_SHIB(RenderManager);
};

NS_END
