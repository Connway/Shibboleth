/************************************************************************************
Copyright (C) 2015 by Nicholas LaCroix

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

#include "Gleam_IRenderDevice_Direct3D.h"
#include "Gleam_IRenderDevice.h"
#include "Gleam_IncludeD3D11.h"
#include "Gleam_BitArray.h"
#include <Gaff_RefPtr.h>
#include <dxgi.h>

NS_GLEAM

class RenderDeviceD3D : public IRenderDevice, public IRenderDeviceD3D
{
public:
	RenderDeviceD3D(void);
	~RenderDeviceD3D(void);

	AdapterList getDisplayModes(int color_format = DXGI_FORMAT_R8G8B8A8_UNORM);

	bool initThreadData(unsigned int*, unsigned int);
	bool init(const IWindow& window, unsigned int adapter_id, unsigned int display_id, unsigned int display_mode_id, bool vsync = false);
	void destroy(void);

	bool isVsync(unsigned int device, unsigned int output) const;
	void setVsync(bool vsync, unsigned int device, unsigned int output);

	void setClearColor(float r, float g, float b, float a);
	const float* getClearColor(void) const;

	void beginFrame(void);
	void endFrame(void);

	bool resize(const IWindow& window);
	bool handleFocusGained(const IWindow& window);

	void resetRenderState(void);

	bool isDeferred(void) const;
	bool isD3D(void) const;

	unsigned int getViewportWidth(unsigned int device, unsigned int output) const;
	unsigned int getViewportHeight(unsigned int device, unsigned int output) const;

	unsigned int getActiveViewportWidth(void);
	unsigned int getActiveViewportHeight(void);

	unsigned int getNumOutputs(unsigned int device) const;
	unsigned int getNumDevices(void) const;

	IRenderTargetPtr getOutputRenderTarget(unsigned int device, unsigned int output);
	IRenderTargetPtr getActiveOutputRenderTarget(void);

	bool setCurrentOutput(unsigned int output);
	unsigned int getCurrentOutput(void) const;

	bool setCurrentDevice(unsigned int device);
	unsigned int getCurrentDevice(void) const;

	unsigned int getDeviceForAdapter(unsigned int adapter_id) const;

	IRenderDevice* createDeferredRenderDevice(void);
	void executeCommandList(ICommandList* command_list);
	bool finishCommandList(ICommandList* command_list);

	ID3D11DeviceContext* getDeviceContext(unsigned int device);
	ID3D11Device* getDevice(unsigned int device);

	ID3D11DeviceContext* getActiveDeviceContext(void);
	ID3D11Device* getActiveDevice(void);

private:
	struct OutputInfo
	{
		GleamArray<DXGI_MODE_DESC> display_mode_list;
		Gaff::COMRefPtr<IDXGIOutput> output;
	};

	struct AdapterInfo
	{
		wchar_t adapter_name[128];
		GleamArray<OutputInfo> output_info;
		Gaff::COMRefPtr<IDXGIAdapter> adapter;
		unsigned int memory;
	};

	struct Device
	{
		GleamArray< Gaff::COMRefPtr<ID3D11RenderTargetView> > render_targets;
		GleamArray< Gaff::COMRefPtr<IDXGISwapChain> > swap_chains;
		GleamArray<IRenderTargetPtr> gleam_rts;
		GleamBitArray vsync;
		GleamArray<D3D11_VIEWPORT> viewports;

		Gaff::COMRefPtr<ID3D11DeviceContext> context;
		Gaff::COMRefPtr<ID3D11Device> device;

		unsigned int adapter_id;
	};

	GleamArray<AdapterInfo> _display_info;
	GleamArray<Device> _devices;

	wchar_t _video_card_name[128];
	float _clear_color[4];

	Gaff::COMRefPtr<ID3D11RenderTargetView> _active_render_target;
	Gaff::COMRefPtr<ID3D11DeviceContext> _active_context;
	Gaff::COMRefPtr<IDXGISwapChain> _active_swap_chain;
	Gaff::COMRefPtr<ID3D11Device> _active_device;
	D3D11_VIEWPORT _active_viewport;
	bool _active_vsync;

	unsigned int _video_memory;

	unsigned int _curr_output;
	unsigned int _curr_device;
};

NS_END
