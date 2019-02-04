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

#include "Gleam_Shader_Direct3D11.h"
#include "Gleam_RenderDevice_Direct3D11.h"
#include "Gleam_IRenderDevice.h"
#include "Gleam_String.h"
#include <d3dcompiler.h>
#include <Gaff_File.h>

#ifdef _DEBUG
	#define SHADER_FLAGS D3DCOMPILE_DEBUG | D3DCOMPILE_ENABLE_STRICTNESS | D3DCOMPILE_SKIP_OPTIMIZATION | D3DCOMPILE_WARNINGS_ARE_ERRORS | D3DCOMPILE_PACK_MATRIX_COLUMN_MAJOR
#else
	#define SHADER_FLAGS D3DCOMPILE_ENABLE_STRICTNESS | D3DCOMPILE_OPTIMIZATION_LEVEL3 | D3DCOMPILE_WARNINGS_ARE_ERRORS | D3DCOMPILE_PACK_MATRIX_COLUMN_MAJOR
#endif

NS_GLEAM

typedef bool (ShaderD3D11::*ShaderInitFunc)(IRenderDevice&, const char*);
static ShaderInitFunc g_init_funcs[IShader::SHADER_TYPE_SIZE] = {
	&ShaderD3D11::initVertex,
	&ShaderD3D11::initPixel,
	&ShaderD3D11::initDomain,
	&ShaderD3D11::initGeometry,
	&ShaderD3D11::initHull,
	&ShaderD3D11::initCompute
};

typedef bool (ShaderD3D11::*ShaderInitSourceFunc)(IRenderDevice&, const char*, size_t);
static ShaderInitSourceFunc g_source_init_funcs[IShader::SHADER_TYPE_SIZE] = {
	&ShaderD3D11::initVertexSource,
	&ShaderD3D11::initPixelSource,
	&ShaderD3D11::initDomainSource,
	&ShaderD3D11::initGeometrySource,
	&ShaderD3D11::initHullSource,
	&ShaderD3D11::initComputeSource
};

ShaderD3D11::ShaderD3D11(void):
	_shader(nullptr), _shader_buffer(nullptr)
{
}

ShaderD3D11::~ShaderD3D11(void)
{
	destroy();
}

bool ShaderD3D11::initSource(IRenderDevice& rd, const char* shader_source, size_t source_size, ShaderType shader_type)
{
	GAFF_ASSERT(shader_type < SHADER_TYPE_SIZE);
	return (this->*g_source_init_funcs[shader_type])(rd, shader_source, source_size);
}

bool ShaderD3D11::initSource(IRenderDevice& rd, const char* shader_source, ShaderType shader_type)
{
	GAFF_ASSERT(shader_type < SHADER_TYPE_SIZE);
	return (this->*g_source_init_funcs[shader_type])(rd, shader_source, strlen(shader_source));
}

bool ShaderD3D11::init(IRenderDevice& rd, const char* file_path, ShaderType shader_type)
{
	GAFF_ASSERT(shader_type < SHADER_TYPE_SIZE);
	return (this->*g_init_funcs[shader_type])(rd, file_path);
}

bool ShaderD3D11::initVertex(IRenderDevice& rd, const char* file_path)
{
	GAFF_ASSERT(rd.getRendererType() == RENDERER_DIRECT3D11 && file_path);

	char* shader_src = nullptr;
	SIZE_T shader_size = 0;

	if (!loadFile(file_path, shader_src, shader_size)) {
		return false;
	}

	_shader_buffer = compileShader(shader_src, shader_size, "VertexMain", "vs_5_0");

	GleamFree(shader_src);

	if (!_shader_buffer) {
		return false;
	}

	RenderDeviceD3D11& rd3d = static_cast<RenderDeviceD3D11&>(rd);
	ID3D11Device5* const device = rd3d.getDevice();

	HRESULT result = device->CreateVertexShader(_shader_buffer->GetBufferPointer(), _shader_buffer->GetBufferSize(), NULL, &_shader_vertex);

	_type = SHADER_VERTEX;

	return SUCCEEDED(result);
}

bool ShaderD3D11::initPixel(IRenderDevice& rd, const char* file_path)
{
	GAFF_ASSERT(rd.getRendererType() == RENDERER_DIRECT3D11 && file_path);

	char* shader_src = nullptr;
	SIZE_T shader_size = 0;

	if (!loadFile(file_path, shader_src, shader_size)) {
		return false;
	}

	_shader_buffer = compileShader(shader_src, shader_size, "PixelMain", "ps_5_0");

	GleamFree(shader_src);

	if (!_shader_buffer) {
		return false;
	}

	RenderDeviceD3D11& rd3d = static_cast<RenderDeviceD3D11&>(rd);
	ID3D11Device5* const device = rd3d.getDevice();

	HRESULT result = device->CreatePixelShader(_shader_buffer->GetBufferPointer(), _shader_buffer->GetBufferSize(), NULL, &_shader_pixel);

	_type = SHADER_PIXEL;

	return SUCCEEDED(result);
}

bool ShaderD3D11::initDomain(IRenderDevice& rd, const char* file_path)
{
	GAFF_ASSERT(rd.getRendererType() == RENDERER_DIRECT3D11 && file_path);

	char* shader_src = nullptr;
	SIZE_T shader_size = 0;

	if (!loadFile(file_path, shader_src, shader_size)) {
		return false;
	}

	_shader_buffer = compileShader(shader_src, shader_size, "DomainMain", "ds_5_0");

	GleamFree(shader_src);

	if (!_shader_buffer) {
		return false;
	}

	RenderDeviceD3D11& rd3d = static_cast<RenderDeviceD3D11&>(rd);
	ID3D11Device5* const device = rd3d.getDevice();

	HRESULT result = device->CreateDomainShader(_shader_buffer->GetBufferPointer(), _shader_buffer->GetBufferSize(), NULL, &_shader_domain);

	_type = SHADER_DOMAIN;

	return SUCCEEDED(result);
}

bool ShaderD3D11::initGeometry(IRenderDevice& rd, const char* file_path)
{
	GAFF_ASSERT(rd.getRendererType() == RENDERER_DIRECT3D11 && file_path);

	char* shader_src = nullptr;
	SIZE_T shader_size = 0;

	if (!loadFile(file_path, shader_src, shader_size)) {
		return false;
	}

	_shader_buffer = compileShader(shader_src, shader_size, "GeometryMain", "gs_5_0");

	GleamFree(shader_src);

	if (!_shader_buffer) {
		return false;
	}

	RenderDeviceD3D11& rd3d = static_cast<RenderDeviceD3D11&>(rd);
	ID3D11Device5* const device = rd3d.getDevice();

	HRESULT result = device->CreateGeometryShader(_shader_buffer->GetBufferPointer(), _shader_buffer->GetBufferSize(), NULL, &_shader_geometry);

	_type = SHADER_GEOMETRY;

	return SUCCEEDED(result);
}

bool ShaderD3D11::initHull(IRenderDevice& rd, const char* file_path)
{
	GAFF_ASSERT(rd.getRendererType() == RENDERER_DIRECT3D11 && file_path);

	char* shader_src = nullptr;
	SIZE_T shader_size = 0;

	if (!loadFile(file_path, shader_src, shader_size)) {
		return false;
	}

	_shader_buffer = compileShader(shader_src, shader_size, "HullMain", "hs_5_0");

	GleamFree(shader_src);

	if (!_shader_buffer) {
		return false;
	}

	RenderDeviceD3D11& rd3d = static_cast<RenderDeviceD3D11&>(rd);
	ID3D11Device5* const device = rd3d.getDevice();

	HRESULT result = device->CreateHullShader(_shader_buffer->GetBufferPointer(), _shader_buffer->GetBufferSize(), NULL, &_shader_hull);

	_type = SHADER_HULL;

	return SUCCEEDED(result);
}

bool ShaderD3D11::initCompute(IRenderDevice& rd, const char* file_path)
{
	GAFF_ASSERT(rd.getRendererType() == RENDERER_DIRECT3D11 && file_path);

	char* shader_src = nullptr;
	SIZE_T shader_size = 0;

	if (!loadFile(file_path, shader_src, shader_size)) {
		return false;
	}

	_shader_buffer = compileShader(shader_src, shader_size, "ComputeMain", "cs_5_0");
	GleamFree(shader_src);

	if (!_shader_buffer) {
		return false;
	}

	RenderDeviceD3D11& rd3d = static_cast<RenderDeviceD3D11&>(rd);
	ID3D11Device5* const device = rd3d.getDevice();

	HRESULT result = device->CreateComputeShader(_shader_buffer->GetBufferPointer(), _shader_buffer->GetBufferSize(), NULL, &_shader_compute);

	_type = SHADER_COMPUTE;

	return SUCCEEDED(result);
}

bool ShaderD3D11::initVertexSource(IRenderDevice& rd, const char* source, size_t source_size)
{
	GAFF_ASSERT(rd.getRendererType() == RENDERER_DIRECT3D11 && source);

	_shader_buffer = compileShader(source, source_size, "VertexMain", "vs_5_0");

	if (!_shader_buffer) {
		return false;
	}

	RenderDeviceD3D11& rd3d = static_cast<RenderDeviceD3D11&>(rd);
	ID3D11Device5* const device = rd3d.getDevice();

	HRESULT result = device->CreateVertexShader(_shader_buffer->GetBufferPointer(), _shader_buffer->GetBufferSize(), NULL, &_shader_vertex);

	_type = SHADER_VERTEX;

	return SUCCEEDED(result);
}

bool ShaderD3D11::initPixelSource(IRenderDevice& rd, const char* source, size_t source_size)
{
	GAFF_ASSERT(rd.getRendererType() == RENDERER_DIRECT3D11 && source);

	_shader_buffer = compileShader(source, source_size, "PixelMain", "ps_5_0");

	if (!_shader_buffer) {
		return false;
	}

	RenderDeviceD3D11& rd3d = static_cast<RenderDeviceD3D11&>(rd);
	ID3D11Device5* const device = rd3d.getDevice();

	HRESULT result = device->CreatePixelShader(_shader_buffer->GetBufferPointer(), _shader_buffer->GetBufferSize(), NULL, &_shader_pixel);

	_type = SHADER_PIXEL;

	return SUCCEEDED(result);
}

bool ShaderD3D11::initDomainSource(IRenderDevice& rd, const char* source, size_t source_size)
{
	GAFF_ASSERT(rd.getRendererType() == RENDERER_DIRECT3D11 && source);

	_shader_buffer = compileShader(source, source_size, "DomainMain", "ds_5_0");

	if (!_shader_buffer) {
		return false;
	}

	RenderDeviceD3D11& rd3d = static_cast<RenderDeviceD3D11&>(rd);
	ID3D11Device5* const device = rd3d.getDevice();

	HRESULT result = device->CreateDomainShader(_shader_buffer->GetBufferPointer(), _shader_buffer->GetBufferSize(), NULL, &_shader_domain);

	_type = SHADER_DOMAIN;

	return SUCCEEDED(result);
}

bool ShaderD3D11::initGeometrySource(IRenderDevice& rd, const char* source, size_t source_size)
{
	GAFF_ASSERT(rd.getRendererType() == RENDERER_DIRECT3D11 && source);

	_shader_buffer = compileShader(source, source_size, "GeometryMain", "gs_5_0");

	if (!_shader_buffer) {
		return false;
	}

	RenderDeviceD3D11& rd3d = static_cast<RenderDeviceD3D11&>(rd);
	ID3D11Device* const device = rd3d.getDevice();

	HRESULT result = device->CreateGeometryShader(_shader_buffer->GetBufferPointer(), _shader_buffer->GetBufferSize(), NULL, &_shader_geometry);

	_type = SHADER_GEOMETRY;

	return SUCCEEDED(result);
}

bool ShaderD3D11::initHullSource(IRenderDevice& rd, const char* source, size_t source_size)
{
	GAFF_ASSERT(rd.getRendererType() == RENDERER_DIRECT3D11 && source);

	_shader_buffer = compileShader(source, source_size, "HullMain", "hs_5_0");

	if (!_shader_buffer) {
		return false;
	}

	RenderDeviceD3D11& rd3d = static_cast<RenderDeviceD3D11&>(rd);
	ID3D11Device5* const device = rd3d.getDevice();

	HRESULT result = device->CreateHullShader(_shader_buffer->GetBufferPointer(), _shader_buffer->GetBufferSize(), NULL, &_shader_hull);

	_type = SHADER_HULL;

	return SUCCEEDED(result);
}

bool ShaderD3D11::initComputeSource(IRenderDevice& rd, const char* source, size_t source_size)
{
	GAFF_ASSERT(rd.getRendererType() == RENDERER_DIRECT3D11 && source);

	_shader_buffer = compileShader(source, source_size, "ComputeMain", "cs_5_0");

	if (!_shader_buffer) {
		return false;
	}

	RenderDeviceD3D11& rd3d = static_cast<RenderDeviceD3D11&>(rd);
	ID3D11Device5* const device = rd3d.getDevice();

	HRESULT result = device->CreateComputeShader(_shader_buffer->GetBufferPointer(), _shader_buffer->GetBufferSize(), NULL, &_shader_compute);

	_type = SHADER_COMPUTE;

	return SUCCEEDED(result);
}

void ShaderD3D11::destroy(void)
{
	if (_shader) {
		switch (_type) {
			case SHADER_VERTEX:
				_shader_vertex->Release();
				break;

			case SHADER_PIXEL:
				_shader_pixel->Release();
				break;

			case SHADER_DOMAIN:
				_shader_domain->Release();
				break;

			case SHADER_GEOMETRY:
				_shader_geometry->Release();
				break;

			case SHADER_HULL:
				_shader_hull->Release();
				break;

			case SHADER_COMPUTE:
				_shader_compute->Release();
				break;

			default:
				break;
		}

		_shader = nullptr;
	}

	SAFERELEASE(_shader_buffer)
}

RendererType ShaderD3D11::getRendererType(void) const
{
	return RENDERER_DIRECT3D11;
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

bool ShaderD3D11::loadFile(const char* file_path, char*& shader_src, SIZE_T& shader_size) const
{
	GAFF_ASSERT(file_path);

	Gaff::File shader(file_path, Gaff::File::READ_BINARY);

	if (!shader.isOpen()) {
		U8String msg("Failed to open shader file: ");
		msg += file_path;

		PrintfToLog(msg.data(), LOG_ERROR);
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

		PrintfToLog(msg.data(), LOG_ERROR);
		return false;
	}

	return true;
}

ID3DBlob* ShaderD3D11::compileShader(const char* shader_src, SIZE_T shader_size, /*macro, include,*/ LPCSTR entry_point, LPCSTR target)
{
	GAFF_ASSERT(shader_src && entry_point && target);

	if (shader_size == SIZE_T_FAIL) {
		shader_size = strlen(shader_src);
	}

	ID3DBlob* shader_buffer;
	ID3DBlob* error_buffer;

	HRESULT result = D3DCompile(shader_src, shader_size, NULL, NULL /*macros*/, NULL /*include*/,
								entry_point, target, SHADER_FLAGS, 0, &shader_buffer, &error_buffer);

	if (FAILED(result)) {
		if (error_buffer) {
			const char* error_msg = reinterpret_cast<const char*>(error_buffer->GetBufferPointer());
			PrintfToLog(error_msg, LOG_ERROR);

			error_buffer->Release();
		}

		return nullptr;
	}

	return shader_buffer;
}

NS_END

#endif
