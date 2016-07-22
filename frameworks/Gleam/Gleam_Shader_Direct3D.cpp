/************************************************************************************
Copyright (C) 2016 by Nicholas LaCroix

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

#include "Gleam_Shader_Direct3D.h"
#include "Gleam_IRenderDevice_Direct3D.h"
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

typedef bool (ShaderD3D::*ShaderInitFunc)(IRenderDevice&, const char*);
static ShaderInitFunc init_funcs[IShader::SHADER_TYPE_SIZE] = {
	&ShaderD3D::initVertex,
	&ShaderD3D::initPixel,
	&ShaderD3D::initDomain,
	&ShaderD3D::initGeometry,
	&ShaderD3D::initHull,
	&ShaderD3D::initCompute
};

typedef bool (ShaderD3D::*ShaderInitSourceFunc)(IRenderDevice&, const char*, size_t);
static ShaderInitSourceFunc source_init_funcs[IShader::SHADER_TYPE_SIZE] = {
	&ShaderD3D::initVertexSource,
	&ShaderD3D::initPixelSource,
	&ShaderD3D::initDomainSource,
	&ShaderD3D::initGeometrySource,
	&ShaderD3D::initHullSource,
	&ShaderD3D::initComputeSource
};

ShaderD3D::ShaderD3D(void):
	_shader(nullptr), _shader_buffer(nullptr)
{
}

ShaderD3D::~ShaderD3D(void)
{
	destroy();
}

bool ShaderD3D::initSource(IRenderDevice& rd, const char* shader_source, size_t source_size, SHADER_TYPE shader_type)
{
	GAFF_ASSERT(shader_type < SHADER_TYPE_SIZE);
	return (this->*source_init_funcs[shader_type])(rd, shader_source, source_size);
}

bool ShaderD3D::initSource(IRenderDevice& rd, const char* shader_source, SHADER_TYPE shader_type)
{
	GAFF_ASSERT(shader_type < SHADER_TYPE_SIZE);
	return (this->*source_init_funcs[shader_type])(rd, shader_source, strlen(shader_source));
}

bool ShaderD3D::init(IRenderDevice& rd, const char* file_path, SHADER_TYPE shader_type)
{
	GAFF_ASSERT(shader_type < SHADER_TYPE_SIZE);
	return (this->*init_funcs[shader_type])(rd, file_path);
}

bool ShaderD3D::initVertex(IRenderDevice& rd, const char* file_path)
{
	GAFF_ASSERT(rd.getRendererType() == RENDERER_DIRECT3D && file_path);

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

	IRenderDeviceD3D& rd3d = reinterpret_cast<IRenderDeviceD3D&>(*(reinterpret_cast<char*>(&rd) + sizeof(IRenderDevice)));

	ID3D11Device* device = rd3d.getActiveDevice();
	HRESULT result = device->CreateVertexShader(_shader_buffer->GetBufferPointer(), _shader_buffer->GetBufferSize(), NULL, &_shader_vertex);

	_type = SHADER_VERTEX;

	return SUCCEEDED(result);
}

bool ShaderD3D::initPixel(IRenderDevice& rd, const char* file_path)
{
	GAFF_ASSERT(rd.getRendererType() == RENDERER_DIRECT3D && file_path);

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

	IRenderDeviceD3D& rd3d = reinterpret_cast<IRenderDeviceD3D&>(*(reinterpret_cast<char*>(&rd) + sizeof(IRenderDevice)));

	ID3D11Device* device = rd3d.getActiveDevice();
	HRESULT result = device->CreatePixelShader(_shader_buffer->GetBufferPointer(), _shader_buffer->GetBufferSize(), NULL, &_shader_pixel);

	_type = SHADER_PIXEL;

	return SUCCEEDED(result);
}

bool ShaderD3D::initDomain(IRenderDevice& rd, const char* file_path)
{
	GAFF_ASSERT(rd.getRendererType() == RENDERER_DIRECT3D && file_path);

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

	IRenderDeviceD3D& rd3d = reinterpret_cast<IRenderDeviceD3D&>(*(reinterpret_cast<char*>(&rd) + sizeof(IRenderDevice)));

	ID3D11Device* device = rd3d.getActiveDevice();
	HRESULT result = device->CreateDomainShader(_shader_buffer->GetBufferPointer(), _shader_buffer->GetBufferSize(), NULL, &_shader_domain);

	_type = SHADER_DOMAIN;

	return SUCCEEDED(result);
}

bool ShaderD3D::initGeometry(IRenderDevice& rd, const char* file_path)
{
	GAFF_ASSERT(rd.getRendererType() == RENDERER_DIRECT3D && file_path);

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

	IRenderDeviceD3D& rd3d = reinterpret_cast<IRenderDeviceD3D&>(*(reinterpret_cast<char*>(&rd) + sizeof(IRenderDevice)));

	ID3D11Device* device = rd3d.getActiveDevice();
	HRESULT result = device->CreateGeometryShader(_shader_buffer->GetBufferPointer(), _shader_buffer->GetBufferSize(), NULL, &_shader_geometry);

	_type = SHADER_GEOMETRY;

	return SUCCEEDED(result);
}

bool ShaderD3D::initHull(IRenderDevice& rd, const char* file_path)
{
	GAFF_ASSERT(rd.getRendererType() == RENDERER_DIRECT3D && file_path);

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

	IRenderDeviceD3D& rd3d = reinterpret_cast<IRenderDeviceD3D&>(*(reinterpret_cast<char*>(&rd) + sizeof(IRenderDevice)));

	ID3D11Device* device = rd3d.getActiveDevice();
	HRESULT result = device->CreateHullShader(_shader_buffer->GetBufferPointer(), _shader_buffer->GetBufferSize(), NULL, &_shader_hull);

	_type = SHADER_HULL;

	return SUCCEEDED(result);
}

bool ShaderD3D::initCompute(IRenderDevice& rd, const char* file_path)
{
	GAFF_ASSERT(rd.getRendererType() == RENDERER_DIRECT3D && file_path);

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

	IRenderDeviceD3D& rd3d = reinterpret_cast<IRenderDeviceD3D&>(*(reinterpret_cast<char*>(&rd) + sizeof(IRenderDevice)));

	ID3D11Device* device = rd3d.getActiveDevice();
	HRESULT result = device->CreateComputeShader(_shader_buffer->GetBufferPointer(), _shader_buffer->GetBufferSize(), NULL, &_shader_compute);

	_type = SHADER_COMPUTE;

	return SUCCEEDED(result);
}

bool ShaderD3D::initVertexSource(IRenderDevice& rd, const char* source, size_t source_size)
{
	GAFF_ASSERT(rd.getRendererType() == RENDERER_DIRECT3D && source);

	_shader_buffer = compileShader(source, source_size, "VertexMain", "vs_5_0");

	if (!_shader_buffer) {
		return false;
	}

	IRenderDeviceD3D& rd3d = reinterpret_cast<IRenderDeviceD3D&>(*(reinterpret_cast<char*>(&rd) + sizeof(IRenderDevice)));

	ID3D11Device* device = rd3d.getActiveDevice();
	HRESULT result = device->CreateVertexShader(_shader_buffer->GetBufferPointer(), _shader_buffer->GetBufferSize(), NULL, &_shader_vertex);

	_type = SHADER_VERTEX;

	return SUCCEEDED(result);
}

bool ShaderD3D::initPixelSource(IRenderDevice& rd, const char* source, size_t source_size)
{
	GAFF_ASSERT(rd.getRendererType() == RENDERER_DIRECT3D && source);

	_shader_buffer = compileShader(source, source_size, "PixelMain", "ps_5_0");

	if (!_shader_buffer) {
		return false;
	}

	IRenderDeviceD3D& rd3d = reinterpret_cast<IRenderDeviceD3D&>(*(reinterpret_cast<char*>(&rd) + sizeof(IRenderDevice)));

	ID3D11Device* device = rd3d.getActiveDevice();
	HRESULT result = device->CreatePixelShader(_shader_buffer->GetBufferPointer(), _shader_buffer->GetBufferSize(), NULL, &_shader_pixel);

	_type = SHADER_PIXEL;

	return SUCCEEDED(result);
}

bool ShaderD3D::initDomainSource(IRenderDevice& rd, const char* source, size_t source_size)
{
	GAFF_ASSERT(rd.getRendererType() == RENDERER_DIRECT3D && source);

	_shader_buffer = compileShader(source, source_size, "DomainMain", "ds_5_0");

	if (!_shader_buffer) {
		return false;
	}

	IRenderDeviceD3D& rd3d = reinterpret_cast<IRenderDeviceD3D&>(*(reinterpret_cast<char*>(&rd) + sizeof(IRenderDevice)));

	ID3D11Device* device = rd3d.getActiveDevice();
	HRESULT result = device->CreateDomainShader(_shader_buffer->GetBufferPointer(), _shader_buffer->GetBufferSize(), NULL, &_shader_domain);

	_type = SHADER_DOMAIN;

	return SUCCEEDED(result);
}

bool ShaderD3D::initGeometrySource(IRenderDevice& rd, const char* source, size_t source_size)
{
	GAFF_ASSERT(rd.getRendererType() == RENDERER_DIRECT3D && source);

	_shader_buffer = compileShader(source, source_size, "GeometryMain", "gs_5_0");

	if (!_shader_buffer) {
		return false;
	}

	IRenderDeviceD3D& rd3d = reinterpret_cast<IRenderDeviceD3D&>(*(reinterpret_cast<char*>(&rd) + sizeof(IRenderDevice)));

	ID3D11Device* device = rd3d.getActiveDevice();
	HRESULT result = device->CreateGeometryShader(_shader_buffer->GetBufferPointer(), _shader_buffer->GetBufferSize(), NULL, &_shader_geometry);

	_type = SHADER_GEOMETRY;

	return SUCCEEDED(result);
}

bool ShaderD3D::initHullSource(IRenderDevice& rd, const char* source, size_t source_size)
{
	GAFF_ASSERT(rd.getRendererType() == RENDERER_DIRECT3D && source);

	_shader_buffer = compileShader(source, source_size, "HullMain", "hs_5_0");

	if (!_shader_buffer) {
		return false;
	}

	IRenderDeviceD3D& rd3d = reinterpret_cast<IRenderDeviceD3D&>(*(reinterpret_cast<char*>(&rd) + sizeof(IRenderDevice)));

	ID3D11Device* device = rd3d.getActiveDevice();
	HRESULT result = device->CreateHullShader(_shader_buffer->GetBufferPointer(), _shader_buffer->GetBufferSize(), NULL, &_shader_hull);

	_type = SHADER_HULL;

	return SUCCEEDED(result);
}

bool ShaderD3D::initComputeSource(IRenderDevice& rd, const char* source, size_t source_size)
{
	GAFF_ASSERT(rd.getRendererType() == RENDERER_DIRECT3D && source);

	_shader_buffer = compileShader(source, source_size, "ComputeMain", "cs_5_0");

	if (!_shader_buffer) {
		return false;
	}

	IRenderDeviceD3D& rd3d = reinterpret_cast<IRenderDeviceD3D&>(*(reinterpret_cast<char*>(&rd) + sizeof(IRenderDevice)));

	ID3D11Device* device = rd3d.getActiveDevice();
	HRESULT result = device->CreateComputeShader(_shader_buffer->GetBufferPointer(), _shader_buffer->GetBufferSize(), NULL, &_shader_compute);

	_type = SHADER_COMPUTE;

	return SUCCEEDED(result);
}

void ShaderD3D::destroy(void)
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
		}

		_shader = nullptr;
	}

	SAFERELEASE(_shader_buffer)
}

RendererType ShaderD3D::getRendererType(void) const
{
	return RENDERER_DIRECT3D;
}

void* ShaderD3D::getShader(void) const
{
	return _shader;
}

ID3D11VertexShader* ShaderD3D::getVertexShader(void) const
{
	return _shader_vertex;
}

ID3D11PixelShader* ShaderD3D::getPixelShader(void) const
{
	return _shader_pixel;
}

ID3D11DomainShader* ShaderD3D::getDomainShader(void) const
{
	return _shader_domain;
}

ID3D11GeometryShader* ShaderD3D::getGeometryShader(void) const
{
	return _shader_geometry;
}

ID3D11HullShader* ShaderD3D::getHullShader(void) const
{
	return _shader_hull;
}

ID3D11ComputeShader* ShaderD3D::getComputeShader(void) const
{
	return _shader_compute;
}

ID3DBlob* ShaderD3D::getByteCodeBuffer(void) const
{
	return _shader_buffer;
}

bool ShaderD3D::loadFile(const char* file_path, char*& shader_src, SIZE_T& shader_size) const
{
	GAFF_ASSERT(file_path);

	Gaff::File shader(file_path, Gaff::File::READ_BINARY);

	if (!shader.isOpen()) {
		GleamAString msg("Failed to open shader file: ");
		msg += file_path;

		PrintfToLog(msg.getBuffer(), LOG_ERROR);
		return false;
	}

	long size = shader.getFileSize();

	shader_size = size;
	shader_src = (char*)GleamAllocate(sizeof(char) * shader_size);

	if (!shader_src || size == -1) {
		return false;
	}

	if (!shader.readEntireFile(shader_src)) {
		GleamFree(shader_src);
		shader_src = nullptr;
		shader_size = 0;

		GleamAString msg("Failed to read shader file: ");
		msg += file_path;

		PrintfToLog(msg.getBuffer(), LOG_ERROR);
		return false;
	}

	return true;
}

ID3DBlob* ShaderD3D::compileShader(const char* shader_src, SIZE_T shader_size, /*macro, include,*/ LPCSTR entry_point, LPCSTR target)
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
