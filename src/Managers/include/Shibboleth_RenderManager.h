/************************************************************************************
Copyright (C) 2016 by Nicholas LaCroix

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

#include "Shibboleth_IRenderManager.h"
#include <Shibboleth_DynamicLoader.h>
#include <Shibboleth_IManager.h>
#include <Shibboleth_Map.h>

NS_SHIBBOLETH

class IApp;

// Maybe call this device manager instead?
class RenderManager : public IManager, public IRenderManager
{
public:
	RenderManager(void);
	~RenderManager(void);

	const char* getName(void) const override;

	bool initThreadData(void) override;
	bool init(const char* module) override;

	Array<RenderDevicePtr>& getDeferredRenderDevices(unsigned int thread_id) override;

	Gleam::IRenderDevice& getRenderDevice(void) override;
	Array<Gaff::SpinLock>& getContextLocks(void) override;

	// Don't call this in a thread sensitive environment
	bool createWindow(
		const char* app_name, Gleam::IWindow::MODE window_mode,
		int x, int y, unsigned int width, unsigned int height,
		unsigned int refresh_rate, const char* device_name,
		unsigned int adapter_id, unsigned int display_id, bool vsync,
		unsigned short tags = 0
	) override;

	void updateWindows(void) override;
	Array<const WindowData*> getWindowsWithTagsAny(unsigned short tags) const override; // Gets windows with any of these tags. If tags is zero, returns all windows.
	Array<const WindowData*> getWindowsWithTags(unsigned short tags) const override; // Gets windows with exactly these tags. If tags is zero, returns all windows.

	Array<unsigned int> getDevicesWithTagsAny(unsigned short tags) const override; // Gets all devices with windows with any of these tags.
	Array<unsigned int> getDevicesWithTags(unsigned short tags) const override; // Gets all devices with windows with exactly these tags.

	size_t getNumWindows(void) const override;
	const WindowData& getWindowData(unsigned int window) const override;
	const Array<WindowData>& getWindowData(void) const override;

	const char* getShaderExtension(void) const override;
	Gleam::IShaderResourceView* createShaderResourceView(void) override;
	Gleam::IDepthStencilState* createDepthStencilState(void) override;
	Gleam::IProgramBuffers* createProgramBuffers(void) override;
	Gleam::IRenderDevice* createRenderDevice(void) override;
	Gleam::IRenderTarget* createRenderTarget(void) override;
	Gleam::ISamplerState* createSamplerState(void) override;
	Gleam::IRasterState* createRasterState(void) override;
	Gleam::ICommandList* createCommandList(void) override;
	Gleam::IBlendState* createBlendState(void) override;
	Gleam::ITexture* createTexture(void) override;
	Gleam::IProgram* createProgram(void) override;
	Gleam::ILayout* createLayout(void) override;
	Gleam::IShader* createShader(void) override;
	Gleam::IBuffer* createBuffer(void) override;
	Gleam::IModel* createModel(void) override;
	Gleam::IMesh* createMesh(void) override;

	// These must be created in the same context as the Windows so that the global input handlers work.
	Gleam::IKeyboard* createKeyboard(void) override;
	Gleam::IMouse* createMouse(void) override;

	WindowRenderTargets createRenderTargetsForEachWindow(void) override;
	Array<RasterStatePtr>& getOrCreateRasterStates(unsigned int hash, const Gleam::IRasterState::RasterStateSettings& settings) override;

private:
	struct GraphicsFunctions
	{
		typedef bool (*InitGraphics)(IApp& app, const char* log_file_name);
		typedef void (*ShutdownGraphics)(void);
		typedef const char* (*GetShaderExtension)(void);
		typedef void (*UpdateWindows)(void);
		typedef Gleam::IWindow* (*CreateWindowS)(void);

		typedef Gleam::IDepthStencilState* (*CreateDepthStencilState)(void);
		typedef Gleam::IShaderResourceView* (*CreateShaderResourceView)(void);
		typedef Gleam::IProgramBuffers* (*CreateProgramBuffers)(void);
		typedef Gleam::IRenderDevice* (*CreateRenderDevice)(void);
		typedef Gleam::IRenderTarget* (*CreateRenderTarget)(void);
		typedef Gleam::ISamplerState* (*CreateSamplerState)(void);
		typedef Gleam::IRasterState* (*CreateRasterState)(void);
		typedef Gleam::ICommandList* (*CreateCommandList)(void);
		typedef Gleam::IBlendState* (*CreateBlendState)(void);
		typedef Gleam::ITexture* (*CreateTexture)(void);
		typedef Gleam::IProgram* (*CreateProgram)(void);
		typedef Gleam::ILayout* (*CreateLayout)(void);
		typedef Gleam::IShader* (*CreateShader)(void);
		typedef Gleam::IBuffer* (*CreateBuffer)(void);
		typedef Gleam::IModel* (*CreateModel)(void);
		typedef Gleam::IMesh* (*CreateMesh)(void);

		using CreateKeyboard = Gleam::IKeyboard* (*)(void);
		using CreateMouse = Gleam::IMouse* (*)(void);

		ShutdownGraphics shutdown;

		UpdateWindows update_windows;
		CreateWindowS create_window;

		GetShaderExtension get_shader_extension;
		CreateDepthStencilState create_depthstencilstate;
		CreateShaderResourceView create_shaderresourceview;
		CreateProgramBuffers create_programbuffers;
		CreateRenderDevice create_renderdevice;
		CreateRenderTarget create_rendertarget;
		CreateSamplerState create_samplerstate;
		CreateRasterState create_rasterstate;
		CreateCommandList create_commandlist;
		CreateBlendState create_blendstate;
		CreateTexture create_texture;
		CreateLayout create_layout;
		CreateProgram create_program;
		CreateShader create_shader;
		CreateBuffer create_buffer;
		CreateModel create_model;
		CreateMesh create_mesh;

		CreateKeyboard create_keyboard;
		CreateMouse create_mouse;
	};

	RenderDevicePtr _render_device;

	GraphicsFunctions _graphics_functions;
	Array<WindowData> _windows;
	Array<size_t> _remove_windows;

	Map< unsigned int, Array<RasterStatePtr> > _raster_states;
	Gaff::SpinLock _rs_lock;

	Map<unsigned int, Array<RenderDevicePtr> > _deferred_devices; // Index is device ID.
	Array<Gaff::SpinLock> _context_locks; // Index is device ID

	DynamicLoader::ModulePtr _gleam_module;

	ProxyAllocator _proxy_allocator;
	IApp& _app;

	int getDisplayModeID(unsigned int width, unsigned int height, unsigned int refresh_rate, unsigned int adapter_id, unsigned int display_id);
	bool cacheGleamFunctions(IApp& app, const char* module);
	bool createWindowRenderable(int width, int height, Gleam::ITexture::FORMAT format, TexturePtr& tex_out, SRVPtr& srv_out);

	bool getDisplayTags(void);

	bool windowMessageHandler(const Gleam::AnyMessage& msg);

	GAFF_NO_COPY(RenderManager);
	GAFF_NO_MOVE(RenderManager);

	SHIB_REF_DEF(RenderManager);
	REF_DEF_REQ;
};

NS_END
