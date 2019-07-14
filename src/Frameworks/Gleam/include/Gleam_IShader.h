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

#pragma once

#include "Gleam_ITexture.h"
#include "Gleam_String.h"
#include <Gaff_Defines.h>

#define MAX_SHADER_VAR 16

NS_GLEAM

class IRenderDevice;

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
};

struct ShaderReflection final
{
	InputParamReflection input_params_reflection[MAX_SHADER_VAR];
	ConstBufferReflection const_buff_reflection[MAX_SHADER_VAR];
	U8String textures[MAX_SHADER_VAR];
	U8String samplers[MAX_SHADER_VAR];
	U8String structured_buffers[MAX_SHADER_VAR];

	int32_t num_inputs = 0;
	int32_t num_constant_buffers = 0;
	int32_t num_textures = 0;
	int32_t num_samplers = 0;
	int32_t num_structured_buffers = 0;
};

class IShader
{
public:
	// Key is RendererType.
	static constexpr const char* g_shader_extensions[] = {
		".hlsl",
		".glsl"
	};

	enum ShaderType
	{
		SHADER_VERTEX = 0,
		SHADER_PIXEL,
		SHADER_DOMAIN,
		SHADER_GEOMETRY,
		SHADER_HULL,
		SHADER_COMPUTE,
		SHADER_TYPE_SIZE,
		SHADER_PIPELINE_COUNT = SHADER_COMPUTE
	};

	virtual ~IShader(void) {}

	virtual bool initSource(IRenderDevice& rd, const char* shader_source, size_t source_size, ShaderType shader_type) = 0;
	virtual bool initSource(IRenderDevice& rd, const char* shader_source, ShaderType shader_type) = 0;
	virtual bool init(IRenderDevice& rd, const char* file_path, ShaderType shader_type) = 0;

	virtual bool initVertex(IRenderDevice&, const char* file_path) = 0;
	virtual bool initPixel(IRenderDevice&, const char* file_path) = 0;
	virtual bool initDomain(IRenderDevice&, const char* file_path) = 0;
	virtual bool initGeometry(IRenderDevice&, const char* file_path) = 0;
	virtual bool initHull(IRenderDevice&, const char* file_path) = 0;
	virtual bool initCompute(IRenderDevice&, const char* file_path) = 0;

	virtual bool initVertexSource(IRenderDevice& rd, const char* source, size_t source_size = SIZE_T_FAIL) = 0;
	virtual bool initPixelSource(IRenderDevice& rd, const char* source, size_t source_size = SIZE_T_FAIL) = 0;
	virtual bool initDomainSource(IRenderDevice& rd, const char* source, size_t source_size = SIZE_T_FAIL) = 0;
	virtual bool initGeometrySource(IRenderDevice& rd, const char* source, size_t source_size = SIZE_T_FAIL) = 0;
	virtual bool initHullSource(IRenderDevice& rd, const char* source, size_t source_size = SIZE_T_FAIL) = 0;
	virtual bool initComputeSource(IRenderDevice& rd, const char* source, size_t source_size = SIZE_T_FAIL) = 0;

	virtual void destroy(void) = 0;

	virtual ShaderReflection getReflectionData(void) const = 0;

	virtual RendererType getRendererType(void) const = 0;

	virtual ShaderType getType(void) const { return _type; }

protected:
	ShaderType _type;
};

NS_END
