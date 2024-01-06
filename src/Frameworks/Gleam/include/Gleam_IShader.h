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

#pragma once

#include "Gleam_ITexture.h"
#include "Gleam_Vector.h"
#include "Gleam_String.h"
#include <Gaff_EnumIterator.h>

NS_GLEAM

class IRenderDevice;
class ILayout;

enum class VarType
{
	Bool,
	Int,
	Float,
	Double,
	String,
	UInt,
	UInt8,

	Count

	//D3D_SVT_BOOL = 1,
	//D3D_SVT_INT = 2,
	//D3D_SVT_FLOAT = 3,
	//D3D_SVT_STRING = 4,
	//D3D_SVT_TEXTURE = 5,
	//D3D_SVT_TEXTURE1D = 6,
	//D3D_SVT_TEXTURE2D = 7,
	//D3D_SVT_TEXTURE3D = 8,
	//D3D_SVT_TEXTURECUBE = 9,
	//D3D_SVT_SAMPLER = 10,
	//D3D_SVT_SAMPLER1D = 11,
	//D3D_SVT_SAMPLER2D = 12,
	//D3D_SVT_SAMPLER3D = 13,
	//D3D_SVT_SAMPLERCUBE = 14,
	//D3D_SVT_PIXELSHADER = 15,
};

struct VarReflection final
{
	U8String name;
	VarType type;
	size_t start_offset = 0;
	int32_t rows = 1;
	int32_t columns = 1;
};

struct ConstBufferReflection final
{
	U8String name;
	size_t size_bytes = 0;
};

struct InputParamReflection final
{
	U8String semantic_name;
	uint32_t semantic_index;
	ITexture::Format format;
	bool instance_data;
};

struct StructuredBufferReflection final
{
	U8String name;
	size_t size_bytes = 0;

	Gleam::Vector<VarReflection> vars;
};

struct ShaderReflection final
{
	Gleam::Vector<InputParamReflection> input_params_reflection;
	Gleam::Vector<ConstBufferReflection> const_buff_reflection;
	Gleam::Vector<U8String> textures;
	Gleam::Vector<U8String> samplers;
	Gleam::Vector<StructuredBufferReflection> structured_buffers;

	Gleam::Vector<U8String> var_decl_order;
};

class IShader
{
public:
	// Key is RendererType.
	static constexpr const char8_t* g_shader_extensions[] = {
		u8".hlsl",
		u8".glsl"
	};

	enum class Type
	{
		Vertex = 0,
		Pixel,
		Domain,
		Geometry,
		Hull,
		Compute,

		Count,
		PipelineCount = Compute
	};

	virtual ~IShader(void) {}

	virtual bool initSource(IRenderDevice& rd, const char* shader_source, size_t source_size, Type shader_type) = 0;
	virtual bool initSource(IRenderDevice& rd, const char* shader_source, Type shader_type) = 0;
	virtual bool init(IRenderDevice& rd, const char8_t* file_path, Type shader_type) = 0;

	virtual bool initVertex(IRenderDevice&, const char8_t* file_path) = 0;
	virtual bool initPixel(IRenderDevice&, const char8_t* file_path) = 0;
	virtual bool initDomain(IRenderDevice&, const char8_t* file_path) = 0;
	virtual bool initGeometry(IRenderDevice&, const char8_t* file_path) = 0;
	virtual bool initHull(IRenderDevice&, const char8_t* file_path) = 0;
	virtual bool initCompute(IRenderDevice&, const char8_t* file_path) = 0;

	virtual bool initVertexSource(IRenderDevice& rd, const char* source, size_t source_size = GAFF_SIZE_T_FAIL) = 0;
	virtual bool initPixelSource(IRenderDevice& rd, const char* source, size_t source_size = GAFF_SIZE_T_FAIL) = 0;
	virtual bool initDomainSource(IRenderDevice& rd, const char* source, size_t source_size = GAFF_SIZE_T_FAIL) = 0;
	virtual bool initGeometrySource(IRenderDevice& rd, const char* source, size_t source_size = GAFF_SIZE_T_FAIL) = 0;
	virtual bool initHullSource(IRenderDevice& rd, const char* source, size_t source_size = GAFF_SIZE_T_FAIL) = 0;
	virtual bool initComputeSource(IRenderDevice& rd, const char* source, size_t source_size = GAFF_SIZE_T_FAIL) = 0;

	virtual void destroy(void) = 0;

	virtual ShaderReflection getReflectionData(void) const = 0;

	virtual RendererType getRendererType(void) const = 0;

	Type getType(void) const { return _type; }

protected:
	Type _type;
};

NS_END

GAFF_DEFINE_ENUM_ITERATOR(Gleam::IShader::Type)
