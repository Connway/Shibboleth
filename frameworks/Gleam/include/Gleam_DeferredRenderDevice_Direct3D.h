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

#include "Gleam_DeferredRenderDeviceBase.h"
#include "Gleam_IRenderDevice_Direct3D.h"
#include "Gleam_IncludeD3D11.h"
#include <Gaff_RefPtr.h>

NS_GLEAM

class DeferredRenderDeviceD3D : public DeferredRenderDeviceBase, public IRenderDeviceD3D
{
public:
	~DeferredRenderDeviceD3D(void);

	void destroy(void);
	bool isDeferred(void) const;
	RendererType getRendererType(void) const;

	const float* getClearColor(void) const;

	void executeCommandList(ICommandList* command_list);
	bool finishCommandList(ICommandList* command_list);

	void renderNoVertexInput(unsigned int vert_count);

	ID3D11DeviceContext* DeferredRenderDeviceD3D::getDeviceContext(unsigned int);
	ID3D11Device* DeferredRenderDeviceD3D::getDevice(unsigned int);
	ID3D11DeviceContext* DeferredRenderDeviceD3D::getActiveDeviceContext(void);
	ID3D11Device* DeferredRenderDeviceD3D::getActiveDevice(void);

private:
	Gaff::COMRefPtr<ID3D11DeviceContext> _context;

	DeferredRenderDeviceD3D(void);
	friend class RenderDeviceD3D;
};

NS_END
