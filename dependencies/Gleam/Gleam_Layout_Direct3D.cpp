/************************************************************************************
Copyright (C) 2014 by Nicholas LaCroix

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

#include "Gleam_Layout_Direct3D.h"
#include "Gleam_RenderDevice_Direct3D.h"
#include "Gleam_Texture_Direct3D.h"
#include "Gleam_Shader_Direct3D.h"
#include "Gleam_Array.h"

NS_GLEAM

static const char* _semantic_names[SEMANTIC_SIZE] = {
	"COLOR",
	"NORMAL",
	"POSITION",
	"TEXCOORD",
	"TANGENT",
	"BITANGENT",
	"BLENDINDICES",
	"BLENDWEIGHT"
};

LayoutD3D::LayoutD3D(void):
	_layout(nullptr)
{
}

LayoutD3D::~LayoutD3D(void)
{
	destroy();
}

bool LayoutD3D::init(IRenderDevice& rd, const LayoutDescription* layout_desc, unsigned int layout_desc_size, const IShader* shader)
{
	assert(rd.isD3D() && shader->isD3D());

	GleamArray<D3D11_INPUT_ELEMENT_DESC> input_desc(layout_desc_size, D3D11_INPUT_ELEMENT_DESC());

	for (unsigned int i = 0; i < layout_desc_size; ++i) {
		input_desc[i].SemanticName = _semantic_names[layout_desc[i].semantic];
		input_desc[i].SemanticIndex = layout_desc[i].semantic_index;
		input_desc[i].Format = TextureD3D::getD3DFormat(layout_desc[i].format);
		input_desc[i].InputSlot = layout_desc[i].input_slot;
		input_desc[i].AlignedByteOffset = layout_desc[i].aligned_byte_offset;
		input_desc[i].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
		input_desc[i].InstanceDataStepRate = 0;
	}

	ID3DBlob* shader_buffer = ((const ShaderD3D*)shader)->getByteCodeBuffer();
	ID3D11Device* device = ((RenderDeviceD3D&)rd).getActiveDevice();

	HRESULT result = device->CreateInputLayout(input_desc.getArray(), layout_desc_size, shader_buffer->GetBufferPointer(), shader_buffer->GetBufferSize(), &_layout);
	return SUCCEEDED(result);
}

void LayoutD3D::destroy(void)
{
	SAFERELEASE(_layout)
}

void LayoutD3D::setLayout(IRenderDevice& rd, const IMesh*)
{
	assert(rd.isD3D());
	((RenderDeviceD3D&)rd).getActiveDeviceContext()->IASetInputLayout(_layout);
}

void LayoutD3D::unsetLayout(IRenderDevice& rd)
{
	assert(rd.isD3D());
	((RenderDeviceD3D&)rd).getActiveDeviceContext()->IASetInputLayout(NULL);
}

NS_END

#endif
