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

#include <Shibboleth_ReflectionDefinitions.h>
#include <Shibboleth_DynamicLoader.h>
#include <Shibboleth_IManager.h>
#include <Shibboleth_Array.h>
#include <Shibboleth_Map.h>
#include <Gleam_IShaderResourceView.h>
#include <Gleam_IRenderTarget.h>
#include <Gleam_ISamplerstate.h>
#include <Gleam_IRasterState.h>
#include <Gleam_ITexture.h>
#include <Gleam_IShader.h>
#include <Gleam_IWindow.h>
#include <Gaff_SmartPtr.h>
#include <Gaff_SpinLock.h>

#define GRAPHICS_CFG "cfg/graphics.cfg"

using RasterStatePtr = Gaff::RefPtr<Gleam::IRasterState>;

#define EXTRACT_DISPLAY_TAGS(display_tags, out_tags) \
	display_tags.forEachInArray([&](size_t, const Gaff::JSON& value) -> bool \
	{ \
		if (!value.isString()) { \
			return true; \
		} \
		out_tags |= GetEnumRefDef<DisplayTags>().getValue(value.getString()); \
		return false; \
	})

namespace Gaff
{
	class JSON;
}

namespace Gleam
{
	class IDepthStencilState;
	class IProgramBuffers;
	class IRenderDevice;
	class ISamplerState;
	class ICommandList;
	class IBlendState;
	class IProgram;
	class ILayout;
	class IShader;
	class IBuffer;
	class IModel;
	class IMesh;
}

NS_SHIBBOLETH

class IApp;

enum DisplayTags
{
	DT_ALL = 0xFFFF,
	DT_1 = 0x0001,
	DT_2 = 0x0002,
	DT_3 = 0x0004,
	DT_4 = 0x0008,
	DT_5 = 0x0010,
	DT_6 = 0x0020,
	DT_7 = 0x0040,
	DT_8 = 0x0080,
	DT_9 = 0x0100,
	DT_10 = 0x0200,
	DT_11 = 0x0400,
	DT_12 = 0x0800,
	DT_13 = 0x1000,
	DT_14 = 0x2000,
	DT_15 = 0x4000,
	DT_16 = 0x8000,
};

enum RenderPasses
{
	RP_OPAQUE = 0,
	RP_TRANSPARENT,
	RP_COUNT
};

SHIB_ENUM_REF_DEF(DisplayTags);
SHIB_ENUM_REF_DEF(RenderPasses);
SHIB_ENUM_REF_DEF_EMBEDDED(Gleam_ISamplerState_Filter, Gleam::ISamplerState::FILTER);
SHIB_ENUM_REF_DEF_EMBEDDED(Gleam_ISamplerState_Wrap, Gleam::ISamplerState::WRAP);
SHIB_ENUM_REF_DEF_EMBEDDED(Gleam_ITexture_Format, Gleam::ITexture::FORMAT);
SHIB_ENUM_REF_DEF_EMBEDDED(Gleam_IShader_Type, Gleam::IShader::SHADER_TYPE);

class RenderManager : public IManager
{
public:
	using RenderDevicePtr = Gaff::SmartPtr<Gleam::IRenderDevice, ProxyAllocator>;
	using RenderTargetPtr = Gaff::RefPtr<Gleam::IRenderTarget>;
	using SRVPtr = Gaff::RefPtr<Gleam::IShaderResourceView>;
	using TexturePtr = Gaff::RefPtr<Gleam::ITexture>;

	struct WindowData
	{
		Gleam::IWindow* window;
		unsigned int device;
		unsigned int output;
		unsigned short tags;
	};

	struct WindowRenderTargets
	{
		Array<RenderTargetPtr> rts;

		Array<TexturePtr> diffuse;
		Array<TexturePtr> specular;
		Array<TexturePtr> normal;
		Array<TexturePtr> position;
		Array<TexturePtr> depth;

		Array<SRVPtr> diffuse_srvs;
		Array<SRVPtr> specular_srvs;
		Array<SRVPtr> normal_srvs;
		Array<SRVPtr> position_srvs;
		Array<SRVPtr> depth_srvs;
	};

	static const char* GetName(void);

	RenderManager(void);
	~RenderManager(void);

	const char* getName(void) const override;

	bool initThreadData(void);
	bool init(const char* module);

	INLINE Array<RenderDevicePtr>& getDeferredRenderDevices(unsigned int thread_id);

	INLINE Gleam::IRenderDevice& getRenderDevice(void);
	INLINE Array<Gaff::SpinLock>& getContextLocks(void);

	// Don't call this in a thread sensitive environment
	bool createWindow(
		const char* app_name, Gleam::IWindow::MODE window_mode,
		int x, int y, unsigned int width, unsigned int height,
		unsigned int refresh_rate, const char* device_name,
		unsigned int adapter_id, unsigned int display_id, bool vsync,
		unsigned short tags = 0
	);

	void updateWindows(void);
	Array<const WindowData*> getWindowsWithTagsAny(unsigned short tags) const; // Gets windows with any of these tags. If tags is zero, returns all windows.
	Array<const WindowData*> getWindowsWithTags(unsigned short tags) const; // Gets windows with exactly these tags. If tags is zero, returns all windows.

	Array<unsigned int> getDevicesWithTagsAny(unsigned short tags) const; // Gets all devices with windows with any of these tags.
	Array<unsigned int> getDevicesWithTags(unsigned short tags) const; // Gets all devices with windows with exactly these tags.

	INLINE size_t getNumWindows(void) const;
	INLINE const WindowData& getWindowData(unsigned int window) const;
	INLINE const Array<WindowData>& getWindowData(void) const;

	INLINE const char* getShaderExtension(void) const;
	INLINE Gleam::IShaderResourceView* createShaderResourceView(void);
	INLINE Gleam::IDepthStencilState* createDepthStencilState(void);
	INLINE Gleam::IProgramBuffers* createProgramBuffers(void);
	INLINE Gleam::IRenderDevice* createRenderDevice(void);
	INLINE Gleam::IRenderTarget* createRenderTarget(void);
	INLINE Gleam::ISamplerState* createSamplerState(void);
	INLINE Gleam::IRasterState* createRasterState(void);
	INLINE Gleam::ICommandList* createCommandList(void);
	INLINE Gleam::IBlendState* createBlendState(void);
	INLINE Gleam::ITexture* createTexture(void);
	INLINE Gleam::IProgram* createProgram(void);
	INLINE Gleam::ILayout* createLayout(void);
	INLINE Gleam::IShader* createShader(void);
	INLINE Gleam::IBuffer* createBuffer(void);
	INLINE Gleam::IModel* createModel(void);
	INLINE Gleam::IMesh* createMesh(void);

	WindowRenderTargets createRenderTargetsForEachWindow(void);
	Array<RasterStatePtr>& getOrCreateRasterStates(unsigned int hash, const Gleam::IRasterState::RasterStateSettings& settings);

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
