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

#ifdef GLEAM_USE_D3D11

#include "Gleam_Layout_Direct3D11.h"
#include "Gleam_RenderDevice_Direct3D11.h"
#include "Gleam_Texture_Direct3D11.h"
#include "Gleam_Shader_Direct3D11.h"
#include "Gleam_IRenderDevice.h"
#include "Gleam_Vector.h"

NS_GLEAM

static const char* _semantic_names[static_cast<size_t>(ILayout::Semantic::Count)] = {
	"COLOR",
	"NORMAL",
	"POSITION",
	"TEXCOORD",
	"TANGENT",
	"BINORMAL",
	"BLENDINDICES",
	"BLENDWEIGHT"
};

Layout::Layout(void):
	_layout(nullptr)
{
}

Layout::~Layout(void)
{
	destroy();
}

bool Layout::init(IRenderDevice& rd, const Description* layout_desc, size_t layout_desc_size, const IShader& shader)
{
	GAFF_ASSERT(rd.getRendererType() == RendererType::Direct3D11 && shader.getRendererType() == RendererType::Direct3D11);

	Vector<D3D11_INPUT_ELEMENT_DESC> input_desc(layout_desc_size, D3D11_INPUT_ELEMENT_DESC());

	for (size_t i = 0; i < layout_desc_size; ++i) {
		input_desc[i].SemanticName = _semantic_names[static_cast<size_t>(layout_desc[i].semantic)];
		input_desc[i].SemanticIndex = static_cast<UINT>(layout_desc[i].semantic_index);
		input_desc[i].Format = Texture::GetD3DFormat(layout_desc[i].format);
		input_desc[i].InputSlot = static_cast<UINT>(layout_desc[i].input_slot);
		input_desc[i].AlignedByteOffset = static_cast<UINT>(layout_desc[i].aligned_byte_offset);
		input_desc[i].InputSlotClass = (layout_desc->per_data_type == PerDataType::Vertex) ? D3D11_INPUT_PER_VERTEX_DATA : D3D11_INPUT_PER_INSTANCE_DATA;
		input_desc[i].InstanceDataStepRate = 0;
	}

	ID3DBlob* shader_buffer = static_cast<const Shader&>(shader).getByteCodeBuffer();
	RenderDevice& rd3d = static_cast<RenderDevice&>(rd);
	ID3D11Device5* const device = rd3d.getDevice();

	const HRESULT result = device->CreateInputLayout(input_desc.data(), static_cast<UINT>(layout_desc_size), shader_buffer->GetBufferPointer(), shader_buffer->GetBufferSize(), &_layout);
	return SUCCEEDED(result);
}

bool Layout::init(IRenderDevice& rd, const IShader& shader)
{
	GAFF_ASSERT(rd.getRendererType() == RendererType::Direct3D11 && shader.getRendererType() == RendererType::Direct3D11);

	const ShaderReflection reflection = shader.getReflectionData();

	Vector<D3D11_INPUT_ELEMENT_DESC> input_desc(reflection.input_params_reflection.size(), D3D11_INPUT_ELEMENT_DESC());

	for (int32_t i = 0; i < static_cast<int32_t>(reflection.input_params_reflection.size()); ++i) {
		const InputParamReflection& input_ref = reflection.input_params_reflection[i];

		input_desc[i].SemanticName = reinterpret_cast<const char*>(input_ref.semantic_name.data());
		input_desc[i].SemanticIndex = input_ref.semantic_index;
		input_desc[i].Format = Texture::GetD3DFormat(input_ref.format);
		input_desc[i].InputSlot = static_cast<UINT>(i);
		input_desc[i].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
		input_desc[i].InputSlotClass = (input_ref.instance_data) ? D3D11_INPUT_PER_INSTANCE_DATA : D3D11_INPUT_PER_VERTEX_DATA;
		input_desc[i].InstanceDataStepRate = 0;
	}

	ID3DBlob* shader_buffer = static_cast<const Shader&>(shader).getByteCodeBuffer();
	RenderDevice& rd3d = static_cast<RenderDevice&>(rd);
	ID3D11Device5* const device = rd3d.getDevice();

	const HRESULT result = device->CreateInputLayout(input_desc.data(), static_cast<UINT>(reflection.input_params_reflection.size()), shader_buffer->GetBufferPointer(), shader_buffer->GetBufferSize(), &_layout);
	return SUCCEEDED(result);
}

void Layout::destroy(void)
{
	GAFF_COM_SAFE_RELEASE(_layout)
}

void Layout::bind(IRenderDevice& rd)
{
	GAFF_ASSERT(rd.getRendererType() == RendererType::Direct3D11);
	RenderDevice& rd3d = static_cast<RenderDevice&>(rd);
	ID3D11DeviceContext3* const context = rd3d.getDeviceContext();
	context->IASetInputLayout(_layout);
}

void Layout::unbind(IRenderDevice& rd)
{
	GAFF_ASSERT(rd.getRendererType() == RendererType::Direct3D11);
	RenderDevice& rd3d = static_cast<RenderDevice&>(rd);
	ID3D11DeviceContext3* const context = rd3d.getDeviceContext();
	context->IASetInputLayout(NULL);
}

RendererType Layout::getRendererType(void) const
{
	return RendererType::Direct3D11;
}

NS_END

#endif
