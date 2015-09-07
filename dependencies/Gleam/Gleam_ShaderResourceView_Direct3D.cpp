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

#include "Gleam_ShaderResourceView_Direct3D.h"
#include "Gleam_IRenderDevice_Direct3D.h"
#include "Gleam_Texture_Direct3D.h"
#include "Gleam_IRenderDevice.h"
#include "Gleam_IncludeD3D11.h"
#include <Gaff_IncludeAssert.h>

NS_GLEAM

static D3D11_SRV_DIMENSION _dimension_map[ITexture::TYPE_SIZE] = {
	D3D11_SRV_DIMENSION_TEXTURE1D,
	D3D11_SRV_DIMENSION_TEXTURE2D,
	D3D11_SRV_DIMENSION_TEXTURE3D,
	D3D11_SRV_DIMENSION_TEXTURECUBE,
	D3D11_SRV_DIMENSION_TEXTURE2D,
	D3D11_SRV_DIMENSION_TEXTURE2D
};

ShaderResourceViewD3D::ShaderResourceViewD3D(void):
	_resource_view(nullptr)
{

}

ShaderResourceViewD3D::~ShaderResourceViewD3D(void)
{
	destroy();
}

bool ShaderResourceViewD3D::init(IRenderDevice& rd, const ITexture* texture)
{
	IRenderDeviceD3D& rd3d = reinterpret_cast<IRenderDeviceD3D&>(*(reinterpret_cast<char*>(&rd) + sizeof(IRenderDevice)));
	ID3D11Device* device = rd3d.getActiveDevice();
	assert(texture);

	_view_type = VIEW_TEXTURE;

	D3D11_SHADER_RESOURCE_VIEW_DESC shader_desc;
	shader_desc.Format = TextureD3D::GetTypedFormat(texture->getFormat());
	shader_desc.ViewDimension = _dimension_map[texture->getType()];
	// the union will set this for all texture types
	shader_desc.Texture2D.MostDetailedMip = 0;
	shader_desc.Texture2D.MipLevels = texture->getMipLevels();

	ID3D11Resource* resource = (ID3D11Resource*)((const TextureD3D*)texture)->getTexture();

	HRESULT result = device->CreateShaderResourceView(resource, &shader_desc, &_resource_view);
	return SUCCEEDED(result);
}

void ShaderResourceViewD3D::destroy(void)
{
	SAFERELEASE(_resource_view)
}

bool ShaderResourceViewD3D::isD3D(void) const
{
	return true;
}

ID3D11ShaderResourceView* ShaderResourceViewD3D::getResourceView(void) const
{
	return _resource_view;
}


NS_END

#endif
