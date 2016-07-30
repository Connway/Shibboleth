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

#include <Shibboleth_GraphicsEnums.h>
#include <Shibboleth_Array.h>
#include <Gleam_IRasterState.h>
#include <Gleam_IWindow.h>
#include <Gaff_SpinLock.h>
#include <Gaff_SmartPtr.h>
#include <Gaff_RefPtr.h>

#define GRAPHICS_CFG "cfg/graphics.cfg"

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
	class IShaderResourceView;
	class IDepthStencilState;
	class IProgramBuffers;
	class IRenderDevice;
	class IRenderTarget;
	class ICommandList;
	class IBlendState;
	class IProgram;
	class ILayout;
	class IBuffer;
	class IModel;
	class IMesh;

	class IKeyboard;
	class IMouse;

	class IWindow;
}

NS_SHIBBOLETH

using RasterStatePtr = Gaff::RefPtr<Gleam::IRasterState>;
using RenderDevicePtr = Gaff::SmartPtr<Gleam::IRenderDevice, ProxyAllocator>;
using RenderTargetPtr = Gaff::RefPtr<Gleam::IRenderTarget>;
using SRVPtr = Gaff::RefPtr<Gleam::IShaderResourceView>;
using TexturePtr = Gaff::RefPtr<Gleam::ITexture>;

class IRenderManager
{
public:
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

	IRenderManager(void) {}
	virtual ~IRenderManager(void) {}

	virtual bool initThreadData(void) = 0;
	virtual bool init(const char* module) = 0;

	virtual Array<RenderDevicePtr>& getDeferredRenderDevices(unsigned int thread_id) = 0;

	virtual Gleam::IRenderDevice& getRenderDevice(void) = 0;
	virtual Array<Gaff::SpinLock>& getContextLocks(void) = 0;

	// Don't call this in a thread sensitive environment
	virtual bool createWindow(
		const char* app_name, Gleam::IWindow::MODE window_mode,
		int x, int y, unsigned int width, unsigned int height,
		unsigned int refresh_rate, const char* device_name,
		unsigned int adapter_id, unsigned int display_id, bool vsync,
		unsigned short tags = 0
	) = 0;

	virtual void updateWindows(void) = 0;
	virtual Array<const WindowData*> getWindowsWithTagsAny(unsigned short tags) const = 0; // Gets windows with any of these tags. If tags is zero, returns all windows.
	virtual Array<const WindowData*> getWindowsWithTags(unsigned short tags) const = 0; // Gets windows with exactly these tags. If tags is zero, returns all windows.

	virtual Array<unsigned int> getDevicesWithTagsAny(unsigned short tags) const = 0; // Gets all devices with windows with any of these tags.
	virtual Array<unsigned int> getDevicesWithTags(unsigned short tags) const = 0; // Gets all devices with windows with exactly these tags.

	virtual size_t getNumWindows(void) const = 0;
	virtual const WindowData& getWindowData(unsigned int window) const = 0;
	virtual const Array<WindowData>& getWindowData(void) const = 0;

	virtual const char* getShaderExtension(void) const = 0;
	virtual Gleam::IShaderResourceView* createShaderResourceView(void) = 0;
	virtual Gleam::IDepthStencilState* createDepthStencilState(void) = 0;
	virtual Gleam::IProgramBuffers* createProgramBuffers(void) = 0;
	virtual Gleam::IRenderDevice* createRenderDevice(void) = 0;
	virtual Gleam::IRenderTarget* createRenderTarget(void) = 0;
	virtual Gleam::ISamplerState* createSamplerState(void) = 0;
	virtual Gleam::IRasterState* createRasterState(void) = 0;
	virtual Gleam::ICommandList* createCommandList(void) = 0;
	virtual Gleam::IBlendState* createBlendState(void) = 0;
	virtual Gleam::ITexture* createTexture(void) = 0;
	virtual Gleam::IProgram* createProgram(void) = 0;
	virtual Gleam::ILayout* createLayout(void) = 0;
	virtual Gleam::IShader* createShader(void) = 0;
	virtual Gleam::IBuffer* createBuffer(void) = 0;
	virtual Gleam::IModel* createModel(void) = 0;
	virtual Gleam::IMesh* createMesh(void) = 0;

	// These must be created in the same context as the Windows so that the global input handlers work.
	virtual Gleam::IKeyboard* createKeyboard(void) = 0;
	virtual Gleam::IMouse* createMouse(void) = 0;

	virtual WindowRenderTargets createRenderTargetsForEachWindow(void) = 0;
	virtual Array<RasterStatePtr>& getOrCreateRasterStates(unsigned int hash, const Gleam::IRasterState::RasterStateSettings& settings) = 0;

	SHIB_INTERFACE_REFLECTION(IRenderManager)
	SHIB_INTERFACE_NAME("Render Manager")
};

NS_END
