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

#include "Direct3D11/Gleam_Texture_Direct3D11.h"
#include "Direct3D11/Gleam_RenderDevice_Direct3D11.h"
#include "Gleam_IRenderDevice.h"
#include <cmath>

NS_GLEAM

static constexpr DXGI_FORMAT _format_map[static_cast<int32_t>(ITexture::Format::SIZE)] = {
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

static constexpr UINT _format_size[static_cast<int32_t>(ITexture::Format::SIZE)] = {
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

DXGI_FORMAT Texture::GetTypelessD3DFormat(Format format)
{
	switch (format) {
		case Format::DEPTH_16_UNORM:
			return DXGI_FORMAT_R16_TYPELESS;

		case Format::DEPTH_32_F:
			return DXGI_FORMAT_R32_TYPELESS;

		case Format::DEPTH_STENCIL_24_8_UNORM_UI:
			return DXGI_FORMAT_R24G8_TYPELESS;

		case Format::DEPTH_STENCIL_32_8_F:
			return DXGI_FORMAT_R32G8X24_TYPELESS;

		default:
			return DXGI_FORMAT_UNKNOWN;
	}
}

DXGI_FORMAT Texture::GetTypedD3DFormat(Format format)
{
	DXGI_FORMAT typed_format;

	switch (format) {
		case Format::DEPTH_16_UNORM:
			typed_format = DXGI_FORMAT_R16_UNORM;
			break;

		case Format::DEPTH_32_F:
			typed_format = DXGI_FORMAT_R32_FLOAT;
			break;

		case Format::DEPTH_STENCIL_24_8_UNORM_UI:
			typed_format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
			break;

		case Format::DEPTH_STENCIL_32_8_F:
			typed_format = DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS;
			break;

		default:
			typed_format = GetD3DFormat(format);
			break;
	}

	return typed_format;
}

DXGI_FORMAT Texture::GetD3DFormat(Format format)
{
	return _format_map[static_cast<int32_t>(format)];
}

Texture::~Texture(void)
{
	destroy();
}

void Texture::destroy(void)
{
	if (_texture) {
		switch (_type) {
			case Type::OneDimensional:
				_texture_1d->Release();
				break;

			case Type::DepthStencil:
			case Type::Depth:
			case Type::TwoDimensional:
				_texture_2d->Release();
				break;

			case Type::ThreeDimensional:
				_texture_3d->Release();
				break;

			case Type::Cube:
				_texture_2d->Release();
				break;

			default:
				break;
		}

		_texture = nullptr;
	}
}

bool Texture::init2DArray(IRenderDevice& rd, int32_t width, int32_t height, Format format, int32_t num_elements, int32_t mip_levels, const void* buffer)
{
	GAFF_ASSERT(width > 0 && height > 0 && mip_levels >= 0 && num_elements > 0);
	GAFF_ASSERT(rd.getRendererType() == RendererType::Direct3D11);

	RenderDevice& rd3d = static_cast<RenderDevice&>(rd);
	ID3D11Device5* const device = rd3d.getDevice();

	_mip_levels = mip_levels;
	_array_size = num_elements;
	_format = format;
	_type = (num_elements > 1) ? Type::TwoDimensionalArray : Type::TwoDimensional;
	_width = width;
	_height = height;
	_depth = 0;

	D3D11_TEXTURE2D_DESC desc;
	desc.Width = static_cast<UINT>(width);
	desc.Height = static_cast<UINT>(height);
	desc.MipLevels = static_cast<UINT>(mip_levels);
	desc.ArraySize = num_elements;
	desc.Format = _format_map[static_cast<int32_t>(format)];
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Usage = (buffer) ? D3D11_USAGE_IMMUTABLE : D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = 0;

	HRESULT result;

	if (buffer) {
		D3D11_SUBRESOURCE_DATA sub;
		sub.pSysMem = buffer;
		sub.SysMemPitch = static_cast<UINT>(width) * _format_size[static_cast<int32_t>(format)];
		sub.SysMemSlicePitch = static_cast<UINT>(width) * static_cast<UINT>(height) * _format_size[static_cast<int32_t>(format)];

		result = device->CreateTexture2D(&desc, &sub, &_texture_2d);

	} else {
		desc.BindFlags |= D3D11_BIND_RENDER_TARGET;
		result = device->CreateTexture2D(&desc, nullptr, &_texture_2d);
	}

	return SUCCEEDED(result);
}

bool Texture::init1DArray(IRenderDevice& rd, int32_t width, Format format, int32_t num_elements, int32_t mip_levels, const void* buffer)
{
	GAFF_ASSERT(width > 0 && mip_levels >= 0 && num_elements > 0);
	GAFF_ASSERT(rd.getRendererType() == RendererType::Direct3D11);

	RenderDevice& rd3d = static_cast<RenderDevice&>(rd);
	ID3D11Device5* const device = rd3d.getDevice();

	_mip_levels = mip_levels;
	_array_size = num_elements;
	_format = format;
	_type = (num_elements > 1) ? Type::OneDimensionalArray : Type::OneDimensional;
	_width = width;
	_height = 0;
	_depth = 0;

	D3D11_TEXTURE1D_DESC desc;
	desc.Width = static_cast<UINT>(width);
	desc.MipLevels = static_cast<UINT>(mip_levels);
	desc.ArraySize = num_elements;
	desc.Format = _format_map[static_cast<int32_t>(format)];
	desc.Usage = (buffer) ? D3D11_USAGE_IMMUTABLE : D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = 0;

	HRESULT result;

	if (buffer) {
		D3D11_SUBRESOURCE_DATA sub;
		sub.pSysMem = buffer;
		sub.SysMemSlicePitch = 0;

		sub.SysMemPitch = static_cast<UINT>(width) * _format_size[static_cast<int32_t>(format)];

		result = device->CreateTexture1D(&desc, &sub, &_texture_1d);

	} else {
		desc.BindFlags |= D3D11_BIND_RENDER_TARGET;
		result = device->CreateTexture1D(&desc, nullptr, &_texture_1d);
	}

	return SUCCEEDED(result);
}

bool Texture::init3D(IRenderDevice& rd, int32_t width, int32_t height, int32_t depth, Format format, int32_t mip_levels, const void* buffer)
{
	GAFF_ASSERT(width > 0 && height > 0 && depth > 0 && mip_levels >= 0);
	GAFF_ASSERT(rd.getRendererType() == RendererType::Direct3D11);

	RenderDevice& rd3d = static_cast<RenderDevice&>(rd);
	ID3D11Device5* const device = rd3d.getDevice();

	_mip_levels = mip_levels;
	_format = format;
	_type = Type::ThreeDimensional;
	_width = width;
	_height = height;
	_depth = depth;

	D3D11_TEXTURE3D_DESC desc;
	desc.Width = static_cast<UINT>(width);
	desc.Height = static_cast<UINT>(height);
	desc.Depth = static_cast<UINT>(depth);
	desc.MipLevels = static_cast<UINT>(mip_levels);
	desc.Format = _format_map[static_cast<int32_t>(format)];
	desc.Usage = (buffer) ? D3D11_USAGE_IMMUTABLE : D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = 0;

	HRESULT result;

	if (buffer) {
		D3D11_SUBRESOURCE_DATA sub;
		sub.pSysMem = buffer;

		sub.SysMemPitch = static_cast<UINT>(width) * _format_size[static_cast<int32_t>(format)];
		sub.SysMemSlicePitch = static_cast<UINT>(width) * static_cast<UINT>(height) * _format_size[static_cast<int32_t>(format)];

		result = device->CreateTexture3D(&desc, &sub, &_texture_3d);

	} else {
		desc.BindFlags |= D3D11_BIND_RENDER_TARGET;
		result = device->CreateTexture3D(&desc, nullptr, &_texture_3d);
	}

	return SUCCEEDED(result);
}

bool Texture::init2D(IRenderDevice& rd, int32_t width, int32_t height, Format format, int32_t mip_levels, const void* buffer)
{
	return init2DArray(rd, width, height, format, 1, mip_levels, buffer);
}

bool Texture::init1D(IRenderDevice& rd, int32_t width, Format format, int32_t mip_levels, const void* buffer)
{
	return init1DArray(rd, width, format, 1, mip_levels, buffer);
}

bool Texture::initCubemap(IRenderDevice& rd, int32_t width, int32_t height, Format format, int32_t mip_levels, const void* buffer)
{
	GAFF_ASSERT(width > 0 && height > 0 && mip_levels >= 0);
	GAFF_ASSERT(rd.getRendererType() == RendererType::Direct3D11);

	RenderDevice& rd3d = static_cast<RenderDevice&>(rd);
	ID3D11Device5* const device = rd3d.getDevice();

	_mip_levels = mip_levels;
	_array_size = 6;
	_format = format;
	_type = Type::Cube;
	_width = width;
	_height = height;
	_depth = 0;

	D3D11_TEXTURE2D_DESC desc;
	desc.Width = static_cast<UINT>(width);
	desc.Height = static_cast<UINT>(height);
	desc.MipLevels = static_cast<UINT>(mip_levels);
	desc.ArraySize = 6;
	desc.Format = _format_map[static_cast<int32_t>(format)];
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Usage = (buffer) ? D3D11_USAGE_IMMUTABLE : D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;

	HRESULT result;

	if (buffer) {
		D3D11_SUBRESOURCE_DATA sub;
		sub.pSysMem = buffer;

		sub.SysMemPitch = static_cast<UINT>(width) * _format_size[static_cast<int32_t>(format)];
		sub.SysMemSlicePitch = static_cast<UINT>(width) * static_cast<UINT>(height) * _format_size[static_cast<int32_t>(format)];

		result = device->CreateTexture2D(&desc, &sub, &_texture_2d);

	} else {
		desc.BindFlags |= D3D11_BIND_RENDER_TARGET;
		result = device->CreateTexture2D(&desc, 0, &_texture_2d);
	}

	return SUCCEEDED(result);
}

bool Texture::initDepthStencil(IRenderDevice& rd, int32_t width, int32_t height, Format format)
{
	GAFF_ASSERT(width > 0 && height > 0);
	GAFF_ASSERT(rd.getRendererType() == RendererType::Direct3D11);

	RenderDevice& rd3d = static_cast<RenderDevice&>(rd);
	ID3D11Device5* const device = rd3d.getDevice();

	switch (format) {
		case Format::DEPTH_16_UNORM:
			_type = Type::Depth;
			break;

		case Format::DEPTH_32_F:
			_type = Type::Depth;
			break;

		case Format::DEPTH_STENCIL_24_8_UNORM_UI:
			_type = Type::DepthStencil;
			break;

		case Format::DEPTH_STENCIL_32_8_F:
			_type = Type::DepthStencil;
			break;

		default:
			_type = Type::Count;
			break;
	}

	_format = format;
	_mip_levels = 1;
	_width = width;
	_height = height;
	_depth = 0;

	GAFF_ASSERT(_type != Type::Count);

	D3D11_TEXTURE2D_DESC depth_stencil_desc;
	depth_stencil_desc.Width = static_cast<UINT>(width);
	depth_stencil_desc.Height = static_cast<UINT>(height);
	depth_stencil_desc.MipLevels = 1;
	depth_stencil_desc.ArraySize = 1;
	depth_stencil_desc.Format = GetTypelessD3DFormat(format);
	depth_stencil_desc.SampleDesc.Count = 1;
	depth_stencil_desc.SampleDesc.Quality = 0;
	depth_stencil_desc.Usage = D3D11_USAGE_DEFAULT;
	depth_stencil_desc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
	depth_stencil_desc.CPUAccessFlags = 0;
	depth_stencil_desc.MiscFlags = 0;

	const HRESULT result = device->CreateTexture2D(&depth_stencil_desc, NULL, &_texture_2d);
	return SUCCEEDED(result);
}

RendererType Texture::getRendererType(void) const
{
	return RendererType::Direct3D11;
}

const void* Texture::getTexture(void) const
{
	return _texture;
}

void* Texture::getTexture(void)
{
	return _texture;
}

const ID3D11Texture1D* Texture::getTexture1D(void) const
{
	return _texture_1d;
}

ID3D11Texture1D* Texture::getTexture1D(void)
{
	return _texture_1d;
}

const ID3D11Texture2D* Texture::getTexture2D(void) const
{
	return _texture_2d;
}

ID3D11Texture2D* Texture::getTexture2D(void)
{
	return _texture_2d;
}

const ID3D11Texture3D* Texture::getTexture3D(void) const
{
	return _texture_3d;
}

ID3D11Texture3D* Texture::getTexture3D(void)
{
	return _texture_3d;
}

NS_END

#endif
