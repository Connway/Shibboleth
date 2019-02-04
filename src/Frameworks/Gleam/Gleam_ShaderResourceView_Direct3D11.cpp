/************************************************************************************
Copyright (C) 2019 by Nicholas LaCroix

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

#include "Gleam_ShaderResourceView_Direct3D11.h"
#include "Gleam_RenderDevice_Direct3D11.h"
#include "Gleam_Texture_Direct3D11.h"
#include "Gleam_Buffer_Direct3D11.h"
#include "Gleam_IRenderDevice.h"
#include "Gleam_IncludeD3D11.h"

NS_GLEAM

static D3D11_SRV_DIMENSION g_dimension_map[ITexture::TYPE_SIZE] = {
	D3D11_SRV_DIMENSION_TEXTURE1D,
	D3D11_SRV_DIMENSION_TEXTURE2D,
	D3D11_SRV_DIMENSION_TEXTURE3D,
	D3D11_SRV_DIMENSION_TEXTURECUBE,
	D3D11_SRV_DIMENSION_TEXTURE2D,
	D3D11_SRV_DIMENSION_TEXTURE2D
};

ShaderResourceViewD3D11::ShaderResourceViewD3D11(void):
	_resource_view(nullptr)
{

}

ShaderResourceViewD3D11::~ShaderResourceViewD3D11(void)
{
	destroy();
}

bool ShaderResourceViewD3D11::init(IRenderDevice& rd, const ITexture* texture)
{
	GAFF_ASSERT(texture);

	RenderDeviceD3D11& rd3d = static_cast<RenderDeviceD3D11&>(rd);
	ID3D11Device5* const device = rd3d.getDevice();

	_view_type = VIEW_TEXTURE;

	D3D11_SHADER_RESOURCE_VIEW_DESC shader_desc;
	shader_desc.Format = TextureD3D11::GetTypedFormat(texture->getFormat());
	shader_desc.ViewDimension = g_dimension_map[texture->getType()];
	// the union will set this for all texture types
	shader_desc.Texture2D.MostDetailedMip = 0;
	shader_desc.Texture2D.MipLevels = texture->getMipLevels();

	ID3D11Resource* resource = static_cast<ID3D11Resource*>(static_cast<const TextureD3D11*>(texture)->getTexture());

	HRESULT result = device->CreateShaderResourceView(resource, &shader_desc, &_resource_view);
	return SUCCEEDED(result);
}

bool ShaderResourceViewD3D11::init(IRenderDevice& rd, const IBuffer* buffer)
{
	GAFF_ASSERT(buffer);

	RenderDeviceD3D11& rd3d = static_cast<RenderDeviceD3D11&>(rd);
	ID3D11Device5* const device = rd3d.getDevice();

	_view_type = VIEW_TEXTURE;

	D3D11_SHADER_RESOURCE_VIEW_DESC shader_desc;
	shader_desc.Format = DXGI_FORMAT_UNKNOWN;
	shader_desc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
	// the union will set this for all texture types
	shader_desc.Buffer.NumElements = static_cast<UINT>(static_cast<int32_t>(buffer->getSize()) / buffer->getStructuredByteStride());
	shader_desc.Buffer.FirstElement = 0;

	ID3D11Resource* resource = static_cast<const BufferD3D11*>(buffer)->getBuffer();

	HRESULT result = device->CreateShaderResourceView(resource, &shader_desc, &_resource_view);
	return SUCCEEDED(result);
}

void ShaderResourceViewD3D11::destroy(void)
{
	SAFERELEASE(_resource_view)
}

RendererType ShaderResourceViewD3D11::getRendererType(void) const
{
	return RENDERER_DIRECT3D11;
}

ID3D11ShaderResourceView* ShaderResourceViewD3D11::getResourceView(void) const
{
	return _resource_view;
}


NS_END

#endif
