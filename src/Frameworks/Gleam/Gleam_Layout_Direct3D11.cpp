/************************************************************************************
Copyright (C) 2018 by Nicholas LaCroix

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

#include "Gleam_Layout_Direct3D11.h"
#include "Gleam_RenderDevice_Direct3D11.h"
#include "Gleam_Texture_Direct3D11.h"
#include "Gleam_Shader_Direct3D11.h"
#include "Gleam_IRenderDevice.h"
#include "Gleam_Vector.h"

NS_GLEAM

static const char* _semantic_names[SEMANTIC_SIZE] = {
	"COLOR",
	"NORMAL",
	"POSITION",
	"TEXCOORD",
	"TANGENT",
	"BINORMAL",
	"BLENDINDICES",
	"BLENDWEIGHT"
};

LayoutD3D11::LayoutD3D11(void):
	_layout(nullptr)
{
}

LayoutD3D11::~LayoutD3D11(void)
{
	destroy();
}

bool LayoutD3D11::init(IRenderDevice& rd, const LayoutDescription* layout_desc, size_t layout_desc_size, const IShader* shader)
{
	GAFF_ASSERT(rd.getRendererType() == RENDERER_DIRECT3D11 && shader->getRendererType() == RENDERER_DIRECT3D11);

	Vector<D3D11_INPUT_ELEMENT_DESC> input_desc(layout_desc_size, D3D11_INPUT_ELEMENT_DESC());

	for (size_t i = 0; i < layout_desc_size; ++i) {
		input_desc[i].SemanticName = _semantic_names[layout_desc[i].semantic];
		input_desc[i].SemanticIndex = static_cast<UINT>(layout_desc[i].semantic_index);
		input_desc[i].Format = TextureD3D11::GetD3DFormat(layout_desc[i].format);
		input_desc[i].InputSlot = static_cast<UINT>(layout_desc[i].input_slot);
		input_desc[i].AlignedByteOffset = static_cast<UINT>(layout_desc[i].aligned_byte_offset);
		input_desc[i].InputSlotClass = (layout_desc->per_data_type == PDT_PER_VERTEX) ? D3D11_INPUT_PER_VERTEX_DATA : D3D11_INPUT_PER_INSTANCE_DATA;
		input_desc[i].InstanceDataStepRate = 0;
	}

	ID3DBlob* shader_buffer = reinterpret_cast<const ShaderD3D11*>(shader)->getByteCodeBuffer();
	RenderDeviceD3D11& rd3d = reinterpret_cast<RenderDeviceD3D11&>(rd);
	ID3D11Device5* const device = rd3d.getDevice();

	HRESULT result = device->CreateInputLayout(input_desc.data(), static_cast<UINT>(layout_desc_size), shader_buffer->GetBufferPointer(), shader_buffer->GetBufferSize(), &_layout);
	return SUCCEEDED(result);
}

void LayoutD3D11::destroy(void)
{
	SAFERELEASE(_layout)
}

void LayoutD3D11::setLayout(IRenderDevice& rd, const IMesh*)
{
	GAFF_ASSERT(rd.getRendererType() == RENDERER_DIRECT3D11);
	RenderDeviceD3D11& rd3d = reinterpret_cast<RenderDeviceD3D11&>(rd);
	ID3D11DeviceContext3* const context = rd3d.getDeviceContext();
	context->IASetInputLayout(_layout);
}

void LayoutD3D11::unsetLayout(IRenderDevice& rd)
{
	GAFF_ASSERT(rd.getRendererType() == RENDERER_DIRECT3D11);
	RenderDeviceD3D11& rd3d = reinterpret_cast<RenderDeviceD3D11&>(rd);
	ID3D11DeviceContext3* const context = rd3d.getDeviceContext();
	context->IASetInputLayout(NULL);
}

RendererType LayoutD3D11::getRendererType(void) const
{
	return RENDERER_DIRECT3D11;
}

NS_END

#endif
