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

#pragma once

#include "Gleam_IRenderDevice_Direct3D11.h"
#include "Gleam_IRenderDevice.h"
#include <Gaff_RefPtr.h>
#include <dxgi1_2.h>

struct ID3D11DeviceContext;
struct ID3D11Device;
struct IDXGIAdapter;

NS_GLEAM

class IRenderOutput;

class RenderDeviceD3D11 : public IRenderDevice, public IRenderDeviceD3D11
{
public:
	bool init(int32_t adapter_id) override;

	void frameBegin(IRenderOutput& output);
	void frameEnd(IRenderOutput& output);

	bool isDeferred(void) const override;
	RendererType getRendererType(void) const override;

	IRenderDevice* createDeferredRenderDevice(void) override;
	void executeCommandList(ICommandList* command_list) override;
	bool finishCommandList(ICommandList* command_list) override;

	void resetRenderState(void) override;
	void renderNoVertexInput(int32_t vert_count) override;

	ID3D11DeviceContext* getDeviceContext(void) override;
	ID3D11Device* getDevice(void) override;
	IDXGIAdapter1* getAdapter(void) override;

private:
	struct OutputInfo
	{
		Vector<DXGI_MODE_DESC> display_mode_list;
	};

	struct AdapterInfo
	{
		wchar_t adapter_name[128];
		Vector<OutputInfo> output_info;
		int32_t memory;
	};

	struct Device
	{
		Gaff::COMRefPtr<ID3D11DeviceContext> context;
		Gaff::COMRefPtr<ID3D11Device> device;
		Gaff::COMRefPtr<IDXGIAdapter1> adapter;

		int32_t adapter_id;
	};

	static Vector<AdapterInfo> g_display_info;
	Device _device;

	friend class RenderOutputD3D11;

	template <RendererType type>
	friend IRenderDevice::AdapterList GetDisplayModes(void);
};

NS_END
