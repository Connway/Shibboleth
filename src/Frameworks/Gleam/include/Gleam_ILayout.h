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

#pragma once

#include "Gleam_ITexture.h"

#define APPEND_ALIGNED 0xFFFFFFFF

NS_GLEAM

class IRenderDevice;
class IShader;
class IMesh;

class ILayout
{
public:
	enum class Semantic
	{
		Color = 0,
		Normal,
		Position,
		TexCoord,
		Tangent,
		Bitangent,
		BlendIndices,
		BlendWeight,

		Count
	};

	enum class PerDataType
	{
		Vertex = 0,
		Instance
	};

	struct Description final
	{
		Semantic semantic;
		int32_t semantic_index;
		ITexture::Format format;
		int32_t input_slot;
		int32_t aligned_byte_offset;
		PerDataType per_data_type;
	};

	ILayout(void) {}
	virtual ~ILayout(void) {}

	virtual bool init(IRenderDevice& rd, const Description* layout_desc, size_t layout_desc_size, const IShader& shader) = 0;
	virtual bool init(IRenderDevice& rd, const IShader& shader) = 0;
	virtual void destroy(void) = 0;

	virtual void bind(IRenderDevice& rd) = 0;
	virtual void unbind(IRenderDevice& rd) = 0;

	virtual RendererType getRendererType(void) const = 0;

	GAFF_NO_COPY(ILayout);
};

NS_END
