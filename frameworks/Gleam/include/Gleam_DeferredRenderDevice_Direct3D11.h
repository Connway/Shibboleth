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

#include "Gleam_DeferredRenderDeviceBase.h"
#include "Gleam_IRenderDevice_Direct3D11.h"
#include "Gleam_IncludeD3D11.h"
#include <Gaff_RefPtr.h>

NS_GLEAM

class DeferredRenderDeviceD3D11 : public DeferredRenderDeviceBase, public IRenderDeviceD3D11
{
public:
	~DeferredRenderDeviceD3D11(void);

	bool isDeferred(void) const;
	RendererType getRendererType(void) const;

	void executeCommandList(ICommandList* command_list);
	bool finishCommandList(ICommandList* command_list);

	//void resetRenderState(void) override;
	void renderNoVertexInput(int32_t vert_count) override;

	ID3D11DeviceContext* getDeviceContext(void) override;
	ID3D11Device* getDevice(void) override;
	IDXGIAdapter1* getAdapter(void) override;

private:
	Gaff::COMRefPtr<ID3D11DeviceContext> _context;

	DeferredRenderDeviceD3D11(void);
	friend class RenderDeviceD3D11;
};

NS_END
