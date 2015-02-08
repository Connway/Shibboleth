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

#if defined(_WIN32) || defined(_WIN64)

#include "Gleam_Texture_Direct3D.h"
#include "Gleam_IRenderDevice_Direct3D.h"
#include "Gleam_IRenderDevice.h"
#include <Gaff_IncludeAssert.h>
#include <cmath>

NS_GLEAM

static DXGI_FORMAT _format_map[ITexture::FORMAT_SIZE] = {
	DXGI_FORMAT_R8_UNORM,
	DXGI_FORMAT_R16_UNORM,
	DXGI_FORMAT_R8G8_UNORM,
	DXGI_FORMAT_R16G16_UNORM,
	//DXGI_FORMAT_R8G8B8A8_UNORM,
	DXGI_FORMAT_R8G8B8A8_UNORM,
	DXGI_FORMAT_R16G16B16A16_UNORM,

	DXGI_FORMAT_R8_SNORM,
	DXGI_FORMAT_R16_SNORM,
	DXGI_FORMAT_R8G8_SNORM,
	DXGI_FORMAT_R16G16_SNORM,
	//DXGI_FORMAT_R8G8B8A8_SNORM,
	DXGI_FORMAT_R16G16B16A16_SNORM,
	DXGI_FORMAT_R8G8B8A8_SNORM,

	DXGI_FORMAT_R8_SINT,
	DXGI_FORMAT_R16_SINT,
	DXGI_FORMAT_R32_SINT,
	DXGI_FORMAT_R8G8_SINT,
	DXGI_FORMAT_R16G16_SINT,
	DXGI_FORMAT_R32G32_SINT,
	//DXGI_FORMAT_R8G8B8A8_SINT,
	//DXGI_FORMAT_R16G16B16A16_SINT,
	DXGI_FORMAT_R32G32B32_SINT,
	DXGI_FORMAT_R8G8B8A8_SINT,
	DXGI_FORMAT_R16G16B16A16_SINT,
	DXGI_FORMAT_R32G32B32A32_SINT,

	DXGI_FORMAT_R8_UINT,
	DXGI_FORMAT_R16_UINT,
	DXGI_FORMAT_R32_UINT,
	DXGI_FORMAT_R8G8_UINT,
	DXGI_FORMAT_R16G16_UINT,
	DXGI_FORMAT_R32G32_UINT,
	//DXGI_FORMAT_R8G8B8A8_UINT,
	//DXGI_FORMAT_R16G16B16A16_UINT,
	DXGI_FORMAT_R32G32B32_UINT,
	DXGI_FORMAT_R8G8B8A8_UINT,
	DXGI_FORMAT_R16G16B16A16_UINT,
	DXGI_FORMAT_R32G32B32A32_UINT,

	DXGI_FORMAT_R16_FLOAT,
	DXGI_FORMAT_R32_FLOAT,
	DXGI_FORMAT_R16G16_FLOAT,
	DXGI_FORMAT_R32G32_FLOAT,
	DXGI_FORMAT_R32G32B32_FLOAT,
	DXGI_FORMAT_R32G32B32_FLOAT,
	DXGI_FORMAT_R16G16B16A16_FLOAT,
	DXGI_FORMAT_R32G32B32A32_FLOAT,

	DXGI_FORMAT_R11G11B10_FLOAT,
	DXGI_FORMAT_R9G9B9E5_SHAREDEXP,
	DXGI_FORMAT_R10G10B10A2_UNORM,
	DXGI_FORMAT_R10G10B10A2_UINT,
	DXGI_FORMAT_R8G8B8A8_UNORM_SRGB,

	DXGI_FORMAT_D16_UNORM,
	DXGI_FORMAT_D32_FLOAT,
	DXGI_FORMAT_D24_UNORM_S8_UINT,
	DXGI_FORMAT_D32_FLOAT_S8X24_UINT
};

static unsigned int _format_size[ITexture::FORMAT_SIZE] = {
	1,
	2,
	2,
	4,
	//3,
	4,
	8,

	1,
	2,
	2,
	4,
	//3,
	6,
	4,

	1,
	2,
	4,
	2,
	4,
	8,
	//3,
	//6,
	12,
	4,
	8,
	16,

	1,
	2,
	4,
	2,
	4,
	8,
	//3,
	//6,
	12,
	4,
	8,
	16,

	2,
	4,
	4,
	8,
	6,
	12,
	8,
	16,

	4,
	4,
	4,
	4,
	4,

	2,
	4,
	4,
	8
};

DXGI_FORMAT TextureD3D::getD3DFormat(FORMAT format)
{
	return _format_map[format];
}

DXGI_FORMAT TextureD3D::getTypedFormat(FORMAT format)
{
	DXGI_FORMAT typed_format;

	switch (format) {
		case DEPTH_16_UNORM:
			typed_format = DXGI_FORMAT_R16_UNORM;
			break;

		case DEPTH_32_F:
			typed_format = DXGI_FORMAT_R32_FLOAT;
			break;

		case DEPTH_STENCIL_24_8_UNORM_UI:
			typed_format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
			break;

		case DEPTH_STENCIL_32_8_F:
			typed_format = DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS;
			break;

		default:
			typed_format = _format_map[format];
			break;
	}

	return typed_format;
}


TextureD3D::TextureD3D(void):
	_depth_stencil_view(nullptr), _texture(nullptr)
{
}

TextureD3D::~TextureD3D(void)
{
}

void TextureD3D::destroy(void)
{
	if (_texture) {
		switch (_type) {
			case ONED:
				_texture_1d->Release();
				break;

			case TWOD:
				_texture_2d->Release();
				break;

			case THREED:
				_texture_3d->Release();
				break;

			case CUBE:
				_texture_2d->Release();
				break;
		}

		_texture = nullptr;
	}
}

bool TextureD3D::init3D(IRenderDevice& rd, int width, int height, int depth, FORMAT format, int mip_levels, const void* buffer)
{
	assert(width > 0 && height > 0 && depth > 0 && mip_levels > 0);
	assert(rd.isD3D());

	IRenderDeviceD3D& rd3d = reinterpret_cast<IRenderDeviceD3D&>(*(reinterpret_cast<char*>(&rd) + sizeof(IRenderDevice)));
	ID3D11Device* device = rd3d.getActiveDevice();

	_mip_levels = (unsigned int)mip_levels;
	_format = format;
	_type = THREED;
	_width = width;
	_height = height;
	_depth = depth;

	D3D11_TEXTURE3D_DESC desc;
	desc.Width = (UINT)width;
	desc.Height = (UINT)height;
	desc.Depth = (UINT)depth;
	desc.MipLevels = (UINT)mip_levels;
	desc.Format = _format_map[format];
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = 0;

	HRESULT result;

	if (buffer) {
		D3D11_SUBRESOURCE_DATA sub;
		sub.pSysMem = buffer;

		sub.SysMemPitch = width * _format_size[format];
		sub.SysMemSlicePitch = width * height * _format_size[format];

		result = device->CreateTexture3D(&desc, &sub, &_texture_3d);

	} else {
		desc.BindFlags |= D3D11_BIND_RENDER_TARGET;
		result = device->CreateTexture3D(&desc, nullptr, &_texture_3d);
	}

	return SUCCEEDED(result);
}

bool TextureD3D::init2D(IRenderDevice& rd, int width, int height, FORMAT format, int mip_levels, const void* buffer)
{
	assert(width > 0 && height > 0 && mip_levels > 0);
	assert(rd.isD3D());

	IRenderDeviceD3D& rd3d = reinterpret_cast<IRenderDeviceD3D&>(*(reinterpret_cast<char*>(&rd) + sizeof(IRenderDevice)));
	ID3D11Device* device = rd3d.getActiveDevice();

	_mip_levels = (unsigned int)mip_levels;
	_format = format;
	_type = TWOD;
	_width = width;
	_height = height;
	_depth = 0;

	D3D11_TEXTURE2D_DESC desc;
	desc.Width = (UINT)width;
	desc.Height = (UINT)height;
	desc.MipLevels = (UINT)mip_levels;
	desc.ArraySize = 1;
	desc.Format = _format_map[format];
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = 0;

	HRESULT result;

	if (buffer) {
		D3D11_SUBRESOURCE_DATA sub;
		sub.pSysMem = buffer;

		sub.SysMemPitch = width * _format_size[format];
		sub.SysMemSlicePitch = width * height * _format_size[format];

		result = device->CreateTexture2D(&desc, &sub, &_texture_2d);

	} else {
		desc.BindFlags |= D3D11_BIND_RENDER_TARGET;
		result = device->CreateTexture2D(&desc, nullptr, &_texture_2d);
	}

	return SUCCEEDED(result);
}

bool TextureD3D::init1D(IRenderDevice& rd, int width, FORMAT format, int mip_levels, const void* buffer)
{
	assert(width > 0 && mip_levels > 0);
	assert(rd.isD3D());

	IRenderDeviceD3D& rd3d = reinterpret_cast<IRenderDeviceD3D&>(*(reinterpret_cast<char*>(&rd) + sizeof(IRenderDevice)));
	ID3D11Device* device = rd3d.getActiveDevice();

	_mip_levels = (unsigned int)mip_levels;
	_format = format;
	_type = ONED;
	_width = width;
	_height = 0;
	_depth = 0;

	D3D11_TEXTURE1D_DESC desc;
	desc.Width = (UINT)width;
	desc.MipLevels = (UINT)mip_levels;
	desc.ArraySize = 1;
	desc.Format = _format_map[format];
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = 0;

	HRESULT result;

	if (buffer) {
		D3D11_SUBRESOURCE_DATA sub;
		sub.pSysMem = buffer;
		sub.SysMemSlicePitch = 0;

		sub.SysMemPitch = width * _format_size[format];

		result = device->CreateTexture1D(&desc, &sub, &_texture_1d);

	} else {
		desc.BindFlags |= D3D11_BIND_RENDER_TARGET;
		result = device->CreateTexture1D(&desc, nullptr, &_texture_1d);
	}

	return SUCCEEDED(result);
}

bool TextureD3D::initCubemap(IRenderDevice& rd, int width, int height, FORMAT format, int mip_levels, const void* buffer)
{
	assert(width > 0 && height > 0 && mip_levels > 0);
	assert(rd.isD3D());

	IRenderDeviceD3D& rd3d = reinterpret_cast<IRenderDeviceD3D&>(*(reinterpret_cast<char*>(&rd) + sizeof(IRenderDevice)));
	ID3D11Device* device = rd3d.getActiveDevice();

	_mip_levels = (unsigned int)mip_levels;
	_format = format;
	_type = CUBE;
	_width = width;
	_height = height;
	_depth = 0;

	D3D11_TEXTURE2D_DESC desc;
	desc.Width = (UINT)width;
	desc.Height = (UINT)height;
	desc.MipLevels = (UINT)mip_levels;
	desc.ArraySize = 6;
	desc.Format = _format_map[format];
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;

	HRESULT result;

	if (buffer) {
		D3D11_SUBRESOURCE_DATA sub;
		sub.pSysMem = buffer;

		sub.SysMemPitch = width * _format_size[format];
		sub.SysMemSlicePitch = width * height * _format_size[format];

		result = device->CreateTexture2D(&desc, &sub, &_texture_2d);

	} else {
		desc.BindFlags |= D3D11_BIND_RENDER_TARGET;
		result = device->CreateTexture2D(&desc, 0, &_texture_2d);
	}

	return SUCCEEDED(result);
}

bool TextureD3D::initDepthStencil(IRenderDevice& rd, int width, int height, FORMAT format)
{
	assert(width > 0 && height > 0);
	assert(rd.isD3D());

	IRenderDeviceD3D& rd3d = reinterpret_cast<IRenderDeviceD3D&>(*(reinterpret_cast<char*>(&rd) + sizeof(IRenderDevice)));
	ID3D11Device* device = rd3d.getActiveDevice();
	DXGI_FORMAT typeless_format = DXGI_FORMAT_R24G8_TYPELESS;

	_format = format;
	_mip_levels = 1;
	_width = width;
	_height = height;
	_depth = 0;

	switch (format) {
		case DEPTH_16_UNORM:
			typeless_format = DXGI_FORMAT_R16_TYPELESS;
			_type = DEPTH;
			break;

		case DEPTH_32_F:
			typeless_format = DXGI_FORMAT_R32_TYPELESS;
			_type = DEPTH;
			break;

		case DEPTH_STENCIL_24_8_UNORM_UI:
			typeless_format = DXGI_FORMAT_R24G8_TYPELESS;
			_type = DEPTH_STENCIL;
			break;

		case DEPTH_STENCIL_32_8_F:
			typeless_format = DXGI_FORMAT_R32G8X24_TYPELESS;
			_type = DEPTH_STENCIL;
			break;

		default:
			_type = TYPE_SIZE;
			break;
	}

	assert(_type != TYPE_SIZE);

	D3D11_TEXTURE2D_DESC depth_stencil_desc;
	depth_stencil_desc.Width = (UINT)width;
	depth_stencil_desc.Height = (UINT)height;
	depth_stencil_desc.MipLevels = 1;
	depth_stencil_desc.ArraySize = 1;
	depth_stencil_desc.Format = typeless_format;
	depth_stencil_desc.SampleDesc.Count = 1;
	depth_stencil_desc.SampleDesc.Quality = 0;
	depth_stencil_desc.Usage = D3D11_USAGE_DEFAULT;
	depth_stencil_desc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
	depth_stencil_desc.CPUAccessFlags = 0;
	depth_stencil_desc.MiscFlags = 0;

	HRESULT result = device->CreateTexture2D(&depth_stencil_desc, NULL, &_texture_2d);
	RETURNIFFAILED(result)

	D3D11_DEPTH_STENCIL_VIEW_DESC ds_view_desc;
	ds_view_desc.Format = _format_map[format];
	ds_view_desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	ds_view_desc.Flags = 0;
	ds_view_desc.Texture2D.MipSlice = 0;

	result = device->CreateDepthStencilView(_texture_2d, &ds_view_desc, &_depth_stencil_view);
	if (FAILED(result)) {
		destroy();
		return false;
	}

	return true;
}

bool TextureD3D::isD3D(void) const
{
	return true;
}

ID3D11DepthStencilView* TextureD3D::getDepthStencilView(void) const
{
	return _depth_stencil_view;
}

void* TextureD3D::getTexture(void) const
{
	return _texture;
}

ID3D11Texture1D* TextureD3D::getTexture1D(void) const
{
	return _texture_1d;
}

ID3D11Texture2D* TextureD3D::getTexture2D(void) const
{
	return _texture_2d;
}

ID3D11Texture3D* TextureD3D::getTexture3D(void) const
{
	return _texture_3d;
}

NS_END

#endif
