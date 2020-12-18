/************************************************************************************
Copyright (C) 2020 by Nicholas LaCroix

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

#include "Gleam_Shader_Direct3D11.h"
#include "Gleam_RenderDevice_Direct3D11.h"
#include "Gleam_IRenderDevice.h"
#include "Gleam_String.h"
#include <Gaff_File.h>
#include <d3dcompiler.h>

#define BASE_SHADER_FLAGS (D3DCOMPILE_ENABLE_STRICTNESS | D3DCOMPILE_WARNINGS_ARE_ERRORS | D3DCOMPILE_PACK_MATRIX_COLUMN_MAJOR | D3DCOMPILE_IEEE_STRICTNESS)

#ifdef _DEBUG
	#define SHADER_FLAGS (BASE_SHADER_FLAGS | D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION)
#else
	#define SHADER_FLAGS (BASE_SHADER_FLAGS | D3DCOMPILE_OPTIMIZATION_LEVEL3)
#endif

NS_GLEAM

using ShaderInitSourceFunc = bool (ShaderD3D11::*)(IRenderDevice&, const char*, size_t);
using ShaderInitFunc = bool (ShaderD3D11::*)(IRenderDevice&, const char*);

static ShaderInitFunc g_init_funcs[static_cast<size_t>(IShader::Type::Count)] = {
	&ShaderD3D11::initVertex,
	&ShaderD3D11::initPixel,
	&ShaderD3D11::initDomain,
	&ShaderD3D11::initGeometry,
	&ShaderD3D11::initHull,
	&ShaderD3D11::initCompute
};

static ShaderInitSourceFunc g_source_init_funcs[static_cast<size_t>(IShader::Type::Count)] = {
	&ShaderD3D11::initVertexSource,
	&ShaderD3D11::initPixelSource,
	&ShaderD3D11::initDomainSource,
	&ShaderD3D11::initGeometrySource,
	&ShaderD3D11::initHullSource,
	&ShaderD3D11::initComputeSource
};

static ITexture::Format GetFormat(const D3D11_SIGNATURE_PARAMETER_DESC& desc)
{
	constexpr uint8_t xyzw = D3D_COMPONENT_MASK_X | D3D_COMPONENT_MASK_Y | D3D_COMPONENT_MASK_Z | D3D_COMPONENT_MASK_W;
	constexpr uint8_t xyz = D3D_COMPONENT_MASK_X | D3D_COMPONENT_MASK_Y | D3D_COMPONENT_MASK_Z;
	constexpr uint8_t xy = D3D_COMPONENT_MASK_X | D3D_COMPONENT_MASK_Y;
	constexpr uint8_t x = D3D_COMPONENT_MASK_X;

	if (desc.ComponentType == D3D_REGISTER_COMPONENT_UINT32) {
		if ((desc.Mask & xyzw) == xyzw) {
			return ITexture::Format::RGBA_32_UI;
		} else if ((desc.Mask & xyz) == xyz) {
			return ITexture::Format::RGB_32_UI;
		} else if ((desc.Mask & xy) == xy) {
			return ITexture::Format::RG_32_UI;
		} else if ((desc.Mask & x) == x) {
			return ITexture::Format::R_32_UI;
		}

	} else if (desc.ComponentType == D3D_REGISTER_COMPONENT_SINT32) {
		if ((desc.Mask & xyzw) == xyzw) {
			return ITexture::Format::RGBA_32_I;
		} else if ((desc.Mask & xyz) == xyz) {
			return ITexture::Format::RGB_32_I;
		} else if ((desc.Mask & xy) == xy) {
			return ITexture::Format::RG_32_I;
		} else if ((desc.Mask & x) == x) {
			return ITexture::Format::R_32_I;
		}

	} else if (desc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32) {
		if ((desc.Mask & xyzw) == xyzw) {
			return ITexture::Format::RGBA_32_F;
		} else if ((desc.Mask & xyz) == xyz) {
			return ITexture::Format::RGB_32_F;
		} else if ((desc.Mask & xy) == xy) {
			return ITexture::Format::RG_32_F;
		} else if ((desc.Mask & x) == x) {
			return ITexture::Format::R_32_F;
		}
	}

	return ITexture::Format::SIZE;
}

static ID3DBlob* CompileShader(const char* shader_src, SIZE_T shader_size, /*macro, include,*/ LPCSTR entry_point, LPCSTR target)
{
	GAFF_ASSERT(shader_src && entry_point && target);

	if (shader_size == SIZE_T_FAIL) {
		shader_size = strlen(shader_src);
	}

	ID3DBlob* shader_buffer;
	ID3DBlob* error_buffer;

	const HRESULT result = D3DCompile(shader_src, shader_size, NULL, NULL /*macros*/, NULL /*include*/,
		entry_point, target, SHADER_FLAGS, 0, &shader_buffer, &error_buffer);

	if (FAILED(result)) {
		if (error_buffer) {
			const char* error_msg = reinterpret_cast<const char*>(error_buffer->GetBufferPointer());
			PrintfToLog(error_msg, LogMsgType::Error);

			error_buffer->Release();
		}

		return nullptr;
	}

	return shader_buffer;
}

static bool LoadFile(const char* file_path, char*& shader_src, SIZE_T& shader_size)
{
	GAFF_ASSERT(file_path);

	Gaff::File shader(file_path, Gaff::File::OM_READ_BINARY);

	if (!shader.isOpen()) {
		U8String msg("Failed to open shader file: ");
		msg += file_path;

		PrintfToLog(msg.data(), LogMsgType::Error);
		return false;
	}

	long size = shader.getFileSize();

	shader_size = size;
	shader_src = reinterpret_cast<char*>(GLEAM_ALLOC(sizeof(char) * shader_size));

	if (!shader_src || size == -1) {
		return false;
	}

	if (!shader.readEntireFile(shader_src)) {
		GleamFree(shader_src);
		shader_src = nullptr;
		shader_size = 0;

		U8String msg("Failed to read shader file: ");
		msg += file_path;

		PrintfToLog(msg.data(), LogMsgType::Error);
		return false;
	}

	return true;
}

static void MakeVarReflection(
	StructuredBufferReflection& out_refl,
	ID3D11ShaderReflectionConstantBuffer* cb_refl)
{
	ID3D11ShaderReflectionVariable* const var_refl = cb_refl->GetVariableByIndex(0);
	ID3D11ShaderReflectionType* const type_refl = var_refl->GetType();
	D3D11_SHADER_TYPE_DESC member_type_desc;
	D3D11_SHADER_TYPE_DESC type_desc;

	type_refl->GetDesc(&type_desc);
	out_refl.vars.resize(type_desc.Members);

	for (int32_t i = 0; i < static_cast<int32_t>(type_desc.Members); ++i) {
		auto& out = out_refl.vars[i];

		ID3D11ShaderReflectionType* const member_type_refl = type_refl->GetMemberTypeByIndex(static_cast<UINT>(i));
		member_type_refl->GetDesc(&member_type_desc);

		out.name = type_refl->GetMemberTypeName(static_cast<UINT>(i));
		out.start_offset = member_type_desc.Offset;

		switch (member_type_desc.Class) {
			case D3D_SVC_MATRIX_COLUMNS:
			case D3D_SVC_MATRIX_ROWS:
			case D3D_SVC_VECTOR:
				out.columns = static_cast<int32_t>(member_type_desc.Columns);
				out.rows = static_cast<int32_t>(member_type_desc.Rows);
				break;

			default:
				break;
		}

		switch (member_type_desc.Type) {
			case D3D_SVT_BOOL:
				out.type = VarType::Bool;
				break;
			case D3D_SVT_INT:
				out.type = VarType::Int;
				break;

			case D3D_SVT_UINT:
				out.type = VarType::UInt;
				break;

			case D3D_SVT_UINT8:
				out.type = VarType::UInt8;
				break;

			case D3D_SVT_FLOAT:
				out.type = VarType::Float;
				break;

			case D3D_SVT_DOUBLE:
				out.type = VarType::Double;
				break;

			case D3D_SVT_STRING:
				out.type = VarType::String;
				break;
		}
	}
}

static void MakeStructuredBufferReflection(
	StructuredBufferReflection& out_refl,
	const D3D11_SHADER_INPUT_BIND_DESC& input_desc,
	const D3D11_SHADER_DESC& shader_desc,
	ID3D11ShaderReflection* refl)
{
	out_refl.name = input_desc.Name;

	for (int32_t i = 0; i < static_cast<int32_t>(shader_desc.ConstantBuffers); ++i) {
		ID3D11ShaderReflectionConstantBuffer* cb_refl = refl->GetConstantBufferByIndex(i);
		D3D11_SHADER_BUFFER_DESC cb_desc;

		cb_refl->GetDesc(&cb_desc);

		if (cb_desc.Name == out_refl.name) {
			out_refl.size_bytes = cb_desc.Size;
			MakeVarReflection(out_refl, cb_refl);
		}
	}
}



ShaderD3D11::ShaderD3D11(void):
	_shader(nullptr), _shader_buffer(nullptr)
{
}

ShaderD3D11::~ShaderD3D11(void)
{
	destroy();
}

bool ShaderD3D11::initSource(IRenderDevice& rd, const char* shader_source, size_t source_size, Type shader_type)
{
	GAFF_ASSERT(static_cast<int32_t>(shader_type) < static_cast<int32_t>(Type::Count));
	return (this->*g_source_init_funcs[static_cast<int32_t>(shader_type)])(rd, shader_source, source_size);
}

bool ShaderD3D11::initSource(IRenderDevice& rd, const char* shader_source, Type shader_type)
{
		GAFF_ASSERT(static_cast<int32_t>(shader_type) < static_cast<int32_t>(Type::Count));
	return (this->*g_source_init_funcs[static_cast<int32_t>(shader_type)])(rd, shader_source, strlen(shader_source));
}

bool ShaderD3D11::init(IRenderDevice& rd, const char* file_path, Type shader_type)
{
		GAFF_ASSERT(static_cast<int32_t>(shader_type) < static_cast<int32_t>(Type::Count));
	return (this->*g_init_funcs[static_cast<int32_t>(shader_type)])(rd, file_path);
}

bool ShaderD3D11::initVertex(IRenderDevice& rd, const char* file_path)
{
	GAFF_ASSERT(rd.getRendererType() == RendererType::DIRECT3D11 && file_path);

	char* shader_src = nullptr;
	SIZE_T shader_size = 0;

	if (!LoadFile(file_path, shader_src, shader_size)) {
		return false;
	}

	_shader_buffer = CompileShader(shader_src, shader_size, "VertexMain", "vs_5_0");

	GleamFree(shader_src);

	if (!_shader_buffer) {
		return false;
	}

	RenderDeviceD3D11& rd3d = static_cast<RenderDeviceD3D11&>(rd);
	ID3D11Device5* const device = rd3d.getDevice();

	const HRESULT result = device->CreateVertexShader(_shader_buffer->GetBufferPointer(), _shader_buffer->GetBufferSize(), NULL, &_shader_vertex);

	_type = Type::Vertex;

	return SUCCEEDED(result);
}

bool ShaderD3D11::initPixel(IRenderDevice& rd, const char* file_path)
{
	GAFF_ASSERT(rd.getRendererType() == RendererType::DIRECT3D11 && file_path);

	char* shader_src = nullptr;
	SIZE_T shader_size = 0;

	if (!LoadFile(file_path, shader_src, shader_size)) {
		return false;
	}

	_shader_buffer = CompileShader(shader_src, shader_size, "PixelMain", "ps_5_0");

	GleamFree(shader_src);

	if (!_shader_buffer) {
		return false;
	}

	RenderDeviceD3D11& rd3d = static_cast<RenderDeviceD3D11&>(rd);
	ID3D11Device5* const device = rd3d.getDevice();

	const HRESULT result = device->CreatePixelShader(_shader_buffer->GetBufferPointer(), _shader_buffer->GetBufferSize(), NULL, &_shader_pixel);

	_type = Type::Pixel;

	return SUCCEEDED(result);
}

bool ShaderD3D11::initDomain(IRenderDevice& rd, const char* file_path)
{
	GAFF_ASSERT(rd.getRendererType() == RendererType::DIRECT3D11 && file_path);

	char* shader_src = nullptr;
	SIZE_T shader_size = 0;

	if (!LoadFile(file_path, shader_src, shader_size)) {
		return false;
	}

	_shader_buffer = CompileShader(shader_src, shader_size, "DomainMain", "ds_5_0");

	GleamFree(shader_src);

	if (!_shader_buffer) {
		return false;
	}

	RenderDeviceD3D11& rd3d = static_cast<RenderDeviceD3D11&>(rd);
	ID3D11Device5* const device = rd3d.getDevice();

	const HRESULT result = device->CreateDomainShader(_shader_buffer->GetBufferPointer(), _shader_buffer->GetBufferSize(), NULL, &_shader_domain);

	_type = Type::Domain;

	return SUCCEEDED(result);
}

bool ShaderD3D11::initGeometry(IRenderDevice& rd, const char* file_path)
{
	GAFF_ASSERT(rd.getRendererType() == RendererType::DIRECT3D11 && file_path);

	char* shader_src = nullptr;
	SIZE_T shader_size = 0;

	if (!LoadFile(file_path, shader_src, shader_size)) {
		return false;
	}

	_shader_buffer = CompileShader(shader_src, shader_size, "GeometryMain", "gs_5_0");

	GleamFree(shader_src);

	if (!_shader_buffer) {
		return false;
	}

	RenderDeviceD3D11& rd3d = static_cast<RenderDeviceD3D11&>(rd);
	ID3D11Device5* const device = rd3d.getDevice();

	const HRESULT result = device->CreateGeometryShader(_shader_buffer->GetBufferPointer(), _shader_buffer->GetBufferSize(), NULL, &_shader_geometry);

	_type = Type::Geometry;

	return SUCCEEDED(result);
}

bool ShaderD3D11::initHull(IRenderDevice& rd, const char* file_path)
{
	GAFF_ASSERT(rd.getRendererType() == RendererType::DIRECT3D11 && file_path);

	char* shader_src = nullptr;
	SIZE_T shader_size = 0;

	if (!LoadFile(file_path, shader_src, shader_size)) {
		return false;
	}

	_shader_buffer = CompileShader(shader_src, shader_size, "HullMain", "hs_5_0");

	GleamFree(shader_src);

	if (!_shader_buffer) {
		return false;
	}

	RenderDeviceD3D11& rd3d = static_cast<RenderDeviceD3D11&>(rd);
	ID3D11Device5* const device = rd3d.getDevice();

	const HRESULT result = device->CreateHullShader(_shader_buffer->GetBufferPointer(), _shader_buffer->GetBufferSize(), NULL, &_shader_hull);

	_type = Type::Hull;

	return SUCCEEDED(result);
}

bool ShaderD3D11::initCompute(IRenderDevice& rd, const char* file_path)
{
	GAFF_ASSERT(rd.getRendererType() == RendererType::DIRECT3D11 && file_path);

	char* shader_src = nullptr;
	SIZE_T shader_size = 0;

	if (!LoadFile(file_path, shader_src, shader_size)) {
		return false;
	}

	_shader_buffer = CompileShader(shader_src, shader_size, "ComputeMain", "cs_5_0");
	GleamFree(shader_src);

	if (!_shader_buffer) {
		return false;
	}

	RenderDeviceD3D11& rd3d = static_cast<RenderDeviceD3D11&>(rd);
	ID3D11Device5* const device = rd3d.getDevice();

	const HRESULT result = device->CreateComputeShader(_shader_buffer->GetBufferPointer(), _shader_buffer->GetBufferSize(), NULL, &_shader_compute);

	_type = Type::Compute;

	return SUCCEEDED(result);
}

bool ShaderD3D11::initVertexSource(IRenderDevice& rd, const char* source, size_t source_size)
{
	GAFF_ASSERT(rd.getRendererType() == RendererType::DIRECT3D11 && source);

	_shader_buffer = CompileShader(source, source_size, "VertexMain", "vs_5_0");

	if (!_shader_buffer) {
		return false;
	}

	RenderDeviceD3D11& rd3d = static_cast<RenderDeviceD3D11&>(rd);
	ID3D11Device5* const device = rd3d.getDevice();

	const HRESULT result = device->CreateVertexShader(_shader_buffer->GetBufferPointer(), _shader_buffer->GetBufferSize(), NULL, &_shader_vertex);

	_type = Type::Vertex;

	return SUCCEEDED(result);
}

bool ShaderD3D11::initPixelSource(IRenderDevice& rd, const char* source, size_t source_size)
{
	GAFF_ASSERT(rd.getRendererType() == RendererType::DIRECT3D11 && source);

	_shader_buffer = CompileShader(source, source_size, "PixelMain", "ps_5_0");

	if (!_shader_buffer) {
		return false;
	}

	RenderDeviceD3D11& rd3d = static_cast<RenderDeviceD3D11&>(rd);
	ID3D11Device5* const device = rd3d.getDevice();

	const HRESULT result = device->CreatePixelShader(_shader_buffer->GetBufferPointer(), _shader_buffer->GetBufferSize(), NULL, &_shader_pixel);

	_type = Type::Pixel;

	return SUCCEEDED(result);
}

bool ShaderD3D11::initDomainSource(IRenderDevice& rd, const char* source, size_t source_size)
{
	GAFF_ASSERT(rd.getRendererType() == RendererType::DIRECT3D11 && source);

	_shader_buffer = CompileShader(source, source_size, "DomainMain", "ds_5_0");

	if (!_shader_buffer) {
		return false;
	}

	RenderDeviceD3D11& rd3d = static_cast<RenderDeviceD3D11&>(rd);
	ID3D11Device5* const device = rd3d.getDevice();

	const HRESULT result = device->CreateDomainShader(_shader_buffer->GetBufferPointer(), _shader_buffer->GetBufferSize(), NULL, &_shader_domain);

	_type = Type::Domain;

	return SUCCEEDED(result);
}

bool ShaderD3D11::initGeometrySource(IRenderDevice& rd, const char* source, size_t source_size)
{
	GAFF_ASSERT(rd.getRendererType() == RendererType::DIRECT3D11 && source);

	_shader_buffer = CompileShader(source, source_size, "GeometryMain", "gs_5_0");

	if (!_shader_buffer) {
		return false;
	}

	RenderDeviceD3D11& rd3d = static_cast<RenderDeviceD3D11&>(rd);
	ID3D11Device* const device = rd3d.getDevice();

	const HRESULT result = device->CreateGeometryShader(_shader_buffer->GetBufferPointer(), _shader_buffer->GetBufferSize(), NULL, &_shader_geometry);

	_type = Type::Geometry;

	return SUCCEEDED(result);
}

bool ShaderD3D11::initHullSource(IRenderDevice& rd, const char* source, size_t source_size)
{
	GAFF_ASSERT(rd.getRendererType() == RendererType::DIRECT3D11 && source);

	_shader_buffer = CompileShader(source, source_size, "HullMain", "hs_5_0");

	if (!_shader_buffer) {
		return false;
	}

	RenderDeviceD3D11& rd3d = static_cast<RenderDeviceD3D11&>(rd);
	ID3D11Device5* const device = rd3d.getDevice();

	const HRESULT result = device->CreateHullShader(_shader_buffer->GetBufferPointer(), _shader_buffer->GetBufferSize(), NULL, &_shader_hull);

	_type = Type::Hull;

	return SUCCEEDED(result);
}

bool ShaderD3D11::initComputeSource(IRenderDevice& rd, const char* source, size_t source_size)
{
	GAFF_ASSERT(rd.getRendererType() == RendererType::DIRECT3D11 && source);

	_shader_buffer = CompileShader(source, source_size, "ComputeMain", "cs_5_0");

	if (!_shader_buffer) {
		return false;
	}

	RenderDeviceD3D11& rd3d = static_cast<RenderDeviceD3D11&>(rd);
	ID3D11Device5* const device = rd3d.getDevice();

	const HRESULT result = device->CreateComputeShader(_shader_buffer->GetBufferPointer(), _shader_buffer->GetBufferSize(), NULL, &_shader_compute);

	_type = Type::Compute;

	return SUCCEEDED(result);
}

void ShaderD3D11::destroy(void)
{
	if (_shader) {
		switch (_type) {
			case Type::Vertex:
				_shader_vertex->Release();
				break;

			case Type::Pixel:
				_shader_pixel->Release();
				break;

			case Type::Domain:
				_shader_domain->Release();
				break;

			case Type::Geometry:
				_shader_geometry->Release();
				break;

			case Type::Hull:
				_shader_hull->Release();
				break;

			case Type::Compute:
				_shader_compute->Release();
				break;

			default:
				break;
		}

		_shader = nullptr;
	}

	SAFERELEASE(_shader_buffer)
}

ShaderReflection ShaderD3D11::getReflectionData(void) const
{
	ShaderReflection reflection;

	ID3D11ShaderReflection* refl = nullptr;

	HRESULT result = D3DReflect(
		_shader_buffer->GetBufferPointer(), _shader_buffer->GetBufferSize(),
		IID_ID3D11ShaderReflection, reinterpret_cast<void**>(&refl)
	);

	if (FAILED(result)) {
		return reflection;
	}

	D3D11_SHADER_DESC shader_desc;
	result = refl->GetDesc(&shader_desc);

	if (FAILED(result)) {
		refl->Release();
		return reflection;
	}

	reflection.input_params_reflection.resize(shader_desc.InputParameters);

	for (int32_t i = 0; i < static_cast<int32_t>(shader_desc.InputParameters); ++i) {
		D3D11_SIGNATURE_PARAMETER_DESC input_desc;
		result = refl->GetInputParameterDesc(i, &input_desc);

		if (FAILED(result)) {
			refl->Release();
			return reflection;
		}

		auto& input_refl = reflection.input_params_reflection[i];
		input_refl.semantic_name = input_desc.SemanticName;
		input_refl.semantic_index = input_desc.SemanticIndex;
		input_refl.format = GetFormat(input_desc);

		input_refl.instance_data = input_desc.SystemValueType == D3D_NAME_INSTANCE_ID;
	}

	for (int32_t i = 0; i < static_cast<int32_t>(shader_desc.ConstantBuffers); ++i) {
		ID3D11ShaderReflectionConstantBuffer* cb_refl = refl->GetConstantBufferByIndex(i);

		D3D11_SHADER_BUFFER_DESC cb_desc;
		result = cb_refl->GetDesc(&cb_desc);

		if (FAILED(result)) {
			refl->Release();
			return reflection;
		}

		if (cb_desc.Type == D3D_CT_RESOURCE_BIND_INFO) {
			continue;
		}

		auto& const_buff_refl = reflection.const_buff_reflection.emplace_back();
		const_buff_refl.name = cb_desc.Name;
		const_buff_refl.size_bytes = cb_desc.Size;
	}

	reflection.const_buff_reflection.shrink_to_fit();

	for (int32_t i = 0; i < static_cast<int32_t>(shader_desc.BoundResources); ++i) {
		D3D11_SHADER_INPUT_BIND_DESC res_desc;
		result = refl->GetResourceBindingDesc(i, &res_desc);

		if (FAILED(result)) {
			refl->Release();
			return reflection;
		}

		switch (res_desc.Type) {
			case D3D_SIT_TEXTURE:
				reflection.textures.emplace_back(res_desc.Name);
				reflection.var_decl_order.emplace_back(res_desc.Name);
				break;

			case D3D_SIT_SAMPLER:
				reflection.samplers.emplace_back(res_desc.Name);
				break;

			case D3D_SIT_STRUCTURED:
				MakeStructuredBufferReflection(
					reflection.structured_buffers.emplace_back(),
					res_desc,
					shader_desc,
					refl
				);

				reflection.var_decl_order.emplace_back(res_desc.Name);
				break;

			default:
				// $TODO: Handle other reflection data.
				break;
		}
	}

	refl->Release();

	return reflection;
}

RendererType ShaderD3D11::getRendererType(void) const
{
	return RendererType::DIRECT3D11;
}

void* ShaderD3D11::getShader(void) const
{
	return _shader;
}

ID3D11VertexShader* ShaderD3D11::getVertexShader(void) const
{
	return _shader_vertex;
}

ID3D11PixelShader* ShaderD3D11::getPixelShader(void) const
{
	return _shader_pixel;
}

ID3D11DomainShader* ShaderD3D11::getDomainShader(void) const
{
	return _shader_domain;
}

ID3D11GeometryShader* ShaderD3D11::getGeometryShader(void) const
{
	return _shader_geometry;
}

ID3D11HullShader* ShaderD3D11::getHullShader(void) const
{
	return _shader_hull;
}

ID3D11ComputeShader* ShaderD3D11::getComputeShader(void) const
{
	return _shader_compute;
}

ID3DBlob* ShaderD3D11::getByteCodeBuffer(void) const
{
	return _shader_buffer;
}

NS_END

#endif
