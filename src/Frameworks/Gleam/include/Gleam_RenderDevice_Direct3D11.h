/************************************************************************************
Copyright (C) 2023 by Nicholas LaCroix

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

#include "Gleam_IRenderDevice.h"
#include "Gleam_IncludeD3D11.h"
#include <Gaff_RefPtr.h>

NS_GLEAM

class RenderDeviceD3D11 final : public IRenderDevice
{
public:
	bool init(int32_t adapter_id) override;

	IRenderDevice* getOwningDevice(void) const override;
	bool isDeferred(void) const override;
	RendererType getRendererType(void) const override;

	IRenderDevice* createDeferredRenderDevice(void) override;
	void executeCommandList(ICommandList& command_list) override;
	bool finishCommandList(ICommandList& command_list) override;

	void clearRenderState(void) override;
	void renderLineNoVertexInputInstanced(int32_t instance_count) override;
	void renderLineNoVertexInput(void) override;
	void renderNoVertexInput(int32_t vert_count) override;

	void setScissorRect(const IVec2& pos, const IVec2& size) override;
	void setScissorRect(const IVec4& rect) override;

	void* getUnderlyingDevice(void) override;

	ID3D11DeviceContext3* getDeviceContext(void);
	ID3D11Device5* getDevice(void);
	IDXGIAdapter4* getAdapter(void);

private:
	struct OutputInfo final
	{
		Vector<DXGI_MODE_DESC1> display_mode_list;
		RECT curr_rect;
		bool is_primary;
		wchar_t display_name[128];
	};

	struct AdapterInfo final
	{
		wchar_t adapter_name[128];
		Vector<OutputInfo> output_info;
		int32_t memory;
	};

	Gaff::COMRefPtr<ID3D11DeviceContext3> _context;
	Gaff::COMRefPtr<ID3D11Device5> _device;
	Gaff::COMRefPtr<IDXGIAdapter4> _adapter;

	RenderDeviceD3D11* _owner = nullptr;

	friend class RenderOutputD3D11;

	template <RendererType type>
	friend IRenderDevice::AdapterList GetDisplayModes(void);
};

NS_END
