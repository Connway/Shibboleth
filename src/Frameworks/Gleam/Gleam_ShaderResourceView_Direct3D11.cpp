/************************************************************************************
Copyright (C) 2024 by Nicholas LaCroix

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

#if GLEAM_USE_D3D11

#include "Gleam_ShaderResourceView_Direct3D11.h"
#include "Gleam_RenderDevice_Direct3D11.h"
#include "Gleam_Texture_Direct3D11.h"
#include "Gleam_Buffer_Direct3D11.h"
#include "Gleam_IRenderDevice.h"
#include "Gleam_IncludeD3D11.h"

NS_GLEAM

static constexpr D3D11_SRV_DIMENSION g_dimension_map[static_cast<int32_t>(ITexture::Type::SIZE)] = {
	D3D11_SRV_DIMENSION_TEXTURE1D,
	D3D11_SRV_DIMENSION_TEXTURE2D,
	D3D11_SRV_DIMENSION_TEXTURE3D,
	D3D11_SRV_DIMENSION_TEXTURECUBE,
	D3D11_SRV_DIMENSION_TEXTURE2D,
	D3D11_SRV_DIMENSION_TEXTURE2D,

	D3D11_SRV_DIMENSION_TEXTURE2DARRAY,
	D3D11_SRV_DIMENSION_TEXTURE1DARRAY
};

ShaderResourceView::ShaderResourceView(void)
{
}

ShaderResourceView::~ShaderResourceView(void)
{
	destroy();
}

bool ShaderResourceView::init(IRenderDevice& rd, const ITexture& texture)
{
	GAFF_ASSERT(texture.getRendererType() == RendererType::Direct3D11);
	GAFF_ASSERT(rd.getRendererType() == RendererType::Direct3D11);

	RenderDevice& rd3d = static_cast<RenderDevice&>(rd);
	ID3D11Device5* const device = rd3d.getDevice();

	D3D11_SHADER_RESOURCE_VIEW_DESC1 res_desc;
	res_desc.Format = Texture::GetTypedD3DFormat(texture.getFormat());
	res_desc.ViewDimension = g_dimension_map[static_cast<int32_t>(texture.getType())];

	// The union will set this for all texture types.
	res_desc.Texture2D.MipLevels = texture.getMipLevels();
	res_desc.Texture2D.MostDetailedMip = 0;

	const ID3D11Resource* resource = nullptr;

	if (texture.getType() == ITexture::Type::CUBE) {
		_view_type = Type::TEXTURE_CUBE;

	} else if (texture.getArraySize() > 1) {
		// 2D array desc is the same structure as the 1D array desc.
		res_desc.Texture2DArray.ArraySize = static_cast<UINT>(texture.getArraySize());
		res_desc.Texture2DArray.FirstArraySlice = 0;
		res_desc.Texture2DArray.PlaneSlice = 0;
		_view_type = Type::TEXTURE_ARRAY;

	} else {
		res_desc.Texture2D.PlaneSlice = 0;
		_view_type = Type::TEXTURE;
	}

	if (texture.getType() == ITexture::Type::THREE_D) {
		resource = static_cast<const Texture&>(texture).getTexture3D();
	} else if (texture.getType() == ITexture::Type::TWO_D || texture.getType() == ITexture::Type::TWO_D_ARRAY || texture.getType() == ITexture::Type::DEPTH || texture.getType() == ITexture::Type::DEPTH_STENCIL) {
		resource = static_cast<const Texture&>(texture).getTexture2D();
	} else 	if (texture.getType() == ITexture::Type::ONE_D || texture.getType() == ITexture::Type::ONE_D_ARRAY) {
		resource = static_cast<const Texture&>(texture).getTexture1D();
	}

	GAFF_ASSERT(resource);

	const HRESULT result = device->CreateShaderResourceView1(const_cast<ID3D11Resource*>(resource), &res_desc, &_resource_view);
	_texture = &texture;

	return SUCCEEDED(result);
}

bool ShaderResourceView::init(IRenderDevice& rd, const IBuffer& buffer, int32_t offset)
{
	GAFF_ASSERT(buffer.getRendererType() == RendererType::Direct3D11);
	GAFF_ASSERT(rd.getRendererType() == RendererType::Direct3D11);

	RenderDevice& rd3d = static_cast<RenderDevice&>(rd);
	ID3D11Device5* const device = rd3d.getDevice();

	_view_type = Type::BUFFER;

	D3D11_SHADER_RESOURCE_VIEW_DESC1 res_desc;
	res_desc.Format = DXGI_FORMAT_UNKNOWN;
	res_desc.ViewDimension = D3D11_SRV_DIMENSION_BUFFEREX;

	res_desc.BufferEx.NumElements = static_cast<UINT>(static_cast<int32_t>(buffer.getSize()) / buffer.getStride());
	res_desc.BufferEx.FirstElement = static_cast<UINT>(offset); // $TODO: This seems to not be working is set to anything other than 0. Possibly related to format being unknown?
	res_desc.BufferEx.Flags = 0;

	ID3D11Resource* const resource = static_cast<const Buffer&>(buffer).getBuffer();
	const HRESULT result = device->CreateShaderResourceView1(resource, &res_desc, &_resource_view);
	_buffer = &buffer;

	return SUCCEEDED(result);
}

void ShaderResourceView::destroy(void)
{
	GAFF_COM_SAFE_RELEASE(_resource_view)
}

RendererType ShaderResourceView::getRendererType(void) const
{
	return RendererType::Direct3D11;
}

ID3D11ShaderResourceView1* ShaderResourceView::getResourceView(void) const
{
	return _resource_view;
}

NS_END

#endif
