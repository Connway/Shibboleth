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

#include "Gleam_DepthStencilState_Direct3D.h"
#include "Gleam_IRenderDevice_Direct3D.h"
#include "Gleam_IRenderDevice.h"
#include "Gleam_IncludeD3D11.h"
#include <Gaff_IncludeAssert.h>

NS_GLEAM

DepthStencilStateD3D::DepthStencilStateD3D(void):
	_depth_stencil_state(nullptr), _stencil_ref(0)
{
}

DepthStencilStateD3D::~DepthStencilStateD3D(void)
{
	destroy();
}

bool DepthStencilStateD3D::init(IRenderDevice& rd, const DepthStencilStateSettings& settings)
{
	assert(rd.isD3D());

	IRenderDeviceD3D& rd3d = reinterpret_cast<IRenderDeviceD3D&>(*(reinterpret_cast<char*>(&rd) + sizeof(IRenderDevice)));
	ID3D11Device* device = rd3d.getActiveDevice();

	D3D11_DEPTH_STENCIL_DESC depth_stencil_desc;
	depth_stencil_desc.DepthEnable = settings.depth_test;
	depth_stencil_desc.StencilEnable = settings.stencil_test;
	depth_stencil_desc.DepthFunc = static_cast<D3D11_COMPARISON_FUNC>(settings.depth_func);
	depth_stencil_desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;

	depth_stencil_desc.StencilReadMask = settings.stencil_read_mask;
	depth_stencil_desc.StencilWriteMask = settings.stencil_write_mask;

	depth_stencil_desc.FrontFace.StencilFunc = static_cast<D3D11_COMPARISON_FUNC>(settings.front_face.comp_func);
	depth_stencil_desc.FrontFace.StencilPassOp = static_cast<D3D11_STENCIL_OP>(settings.front_face.stencil_depth_pass);
	depth_stencil_desc.FrontFace.StencilFailOp = static_cast<D3D11_STENCIL_OP>(settings.front_face.stencil_depth_fail);
	depth_stencil_desc.FrontFace.StencilDepthFailOp = static_cast<D3D11_STENCIL_OP>(settings.front_face.stencil_pass_depth_fail);

	depth_stencil_desc.BackFace.StencilFunc = static_cast<D3D11_COMPARISON_FUNC>(settings.back_face.comp_func);
	depth_stencil_desc.BackFace.StencilPassOp = static_cast<D3D11_STENCIL_OP>(settings.back_face.stencil_depth_pass);
	depth_stencil_desc.BackFace.StencilFailOp = static_cast<D3D11_STENCIL_OP>(settings.back_face.stencil_depth_fail);
	depth_stencil_desc.BackFace.StencilDepthFailOp = static_cast<D3D11_STENCIL_OP>(settings.back_face.stencil_pass_depth_fail);

	_stencil_ref = settings.stencil_ref;

	HRESULT result = device->CreateDepthStencilState(&depth_stencil_desc, &_depth_stencil_state);
	return SUCCEEDED(result);
}

void DepthStencilStateD3D::destroy(void)
{
	SAFERELEASE(_depth_stencil_state);
}

void DepthStencilStateD3D::set(IRenderDevice& rd) const
{
	assert(rd.isD3D());
	IRenderDeviceD3D& rd3d = reinterpret_cast<IRenderDeviceD3D&>(*(reinterpret_cast<char*>(&rd) + sizeof(IRenderDevice)));
	ID3D11DeviceContext* context = rd3d.getActiveDeviceContext();

	context->OMSetDepthStencilState(_depth_stencil_state, _stencil_ref);
}

void DepthStencilStateD3D::unset(IRenderDevice& rd) const
{
	assert(rd.isD3D());
	IRenderDeviceD3D& rd3d = reinterpret_cast<IRenderDeviceD3D&>(*(reinterpret_cast<char*>(&rd) + sizeof(IRenderDevice)));
	ID3D11DeviceContext* context = rd3d.getActiveDeviceContext();

	context->OMSetDepthStencilState(NULL, 0);
}

bool DepthStencilStateD3D::isD3D(void) const
{
	return true;
}

NS_END
