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

#include "Gleam_Shader_Direct3D.h"
#include "Gleam_RenderDevice_Direct3D.h"
#include <d3dcompiler.h>
#include <Gaff_File.h>

#ifdef _DEBUG
	#define SHADER_FLAGS D3DCOMPILE_DEBUG | D3DCOMPILE_ENABLE_STRICTNESS | D3DCOMPILE_SKIP_OPTIMIZATION | D3DCOMPILE_WARNINGS_ARE_ERRORS | D3DCOMPILE_PACK_MATRIX_COLUMN_MAJOR
#else
	#define SHADER_FLAGS D3DCOMPILE_ENABLE_STRICTNESS | D3DCOMPILE_OPTIMIZATION_LEVEL3 | D3DCOMPILE_WARNINGS_ARE_ERRORS | D3DCOMPILE_PACK_MATRIX_COLUMN_MAJOR
#endif

NS_GLEAM

ShaderD3D::ShaderD3D(void):
	_shader(nullptr), _shader_buffer(nullptr)
{
}

ShaderD3D::~ShaderD3D(void)
{
	destroy();
}

bool ShaderD3D::initVertex(IRenderDevice& rd, const GleamGString& file_path)
{
	assert(rd.isD3D() && file_path.size());
	return initVertex(rd, file_path.getBuffer());
}

bool ShaderD3D::initPixel(IRenderDevice& rd, const GleamGString& file_path)
{
	assert(rd.isD3D() && file_path.size());
	return initPixel(rd, file_path.getBuffer());
}

bool ShaderD3D::initDomain(IRenderDevice& rd, const GleamGString& file_path)
{
	assert(rd.isD3D() && file_path.size());
	return initDomain(rd, file_path.getBuffer());
}

bool ShaderD3D::initGeometry(IRenderDevice& rd, const GleamGString& file_path)
{
	assert(rd.isD3D() && file_path.size());
	return initGeometry(rd, file_path.getBuffer());
}

bool ShaderD3D::initHull(IRenderDevice& rd, const GleamGString& file_path)
{
	assert(rd.isD3D() && file_path.size());
	return initHull(rd, file_path.getBuffer());
}

bool ShaderD3D::initCompute(IRenderDevice& rd, const GleamGString& file_path)
{
	assert(rd.isD3D() && file_path.size());
	return initCompute(rd, file_path.getBuffer());
}

bool ShaderD3D::initVertex(IRenderDevice& rd, const GChar* file_path)
{
	assert(rd.isD3D() && file_path);

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

	ID3D11Device* device = ((RenderDeviceD3D&)rd).getActiveDevice();
	HRESULT result = device->CreateVertexShader(_shader_buffer->GetBufferPointer(), _shader_buffer->GetBufferSize(), NULL, &_shader_vertex);

	_type = SHADER_VERTEX;

	return SUCCEEDED(result);
}

bool ShaderD3D::initPixel(IRenderDevice& rd, const GChar* file_path)
{
	assert(rd.isD3D() && file_path);

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

	ID3D11Device* device = ((RenderDeviceD3D&)rd).getActiveDevice();
	HRESULT result = device->CreatePixelShader(_shader_buffer->GetBufferPointer(), _shader_buffer->GetBufferSize(), NULL, &_shader_pixel);

	_type = SHADER_PIXEL;

	return SUCCEEDED(result);
}

bool ShaderD3D::initDomain(IRenderDevice& rd, const GChar* file_path)
{
	assert(rd.isD3D() && file_path);

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

	ID3D11Device* device = ((RenderDeviceD3D&)rd).getActiveDevice();
	HRESULT result = device->CreateDomainShader(_shader_buffer->GetBufferPointer(), _shader_buffer->GetBufferSize(), NULL, &_shader_domain);

	_type = SHADER_DOMAIN;

	return SUCCEEDED(result);
}

bool ShaderD3D::initGeometry(IRenderDevice& rd, const GChar* file_path)
{
	assert(rd.isD3D() && file_path);

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

	ID3D11Device* device = ((RenderDeviceD3D&)rd).getActiveDevice();
	HRESULT result = device->CreateGeometryShader(_shader_buffer->GetBufferPointer(), _shader_buffer->GetBufferSize(), NULL, &_shader_geometry);

	_type = SHADER_GEOMETRY;

	return SUCCEEDED(result);
}

bool ShaderD3D::initHull(IRenderDevice& rd, const GChar* file_path)
{
	assert(rd.isD3D() && file_path);

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

	ID3D11Device* device = ((RenderDeviceD3D&)rd).getActiveDevice();
	HRESULT result = device->CreateHullShader(_shader_buffer->GetBufferPointer(), _shader_buffer->GetBufferSize(), NULL, &_shader_hull);

	_type = SHADER_HULL;

	return SUCCEEDED(result);
}

bool ShaderD3D::initCompute(IRenderDevice& rd, const GChar* file_path)
{
	assert(rd.isD3D() && file_path);

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

	ID3D11Device* device = ((RenderDeviceD3D&)rd).getActiveDevice();
	HRESULT result = device->CreateComputeShader(_shader_buffer->GetBufferPointer(), _shader_buffer->GetBufferSize(), NULL, &_shader_compute);

	_type = SHADER_COMPUTE;

	return SUCCEEDED(result);
}

bool ShaderD3D::initVertexSource(IRenderDevice& rd, const GleamAString& source)
{
	assert(rd.isD3D() && source.size());

	_shader_buffer = compileShader(source.getBuffer(), source.size(), "VertexMain", "vs_5_0");

	if (!_shader_buffer) {
		return false;
	}

	ID3D11Device* device = ((RenderDeviceD3D&)rd).getActiveDevice();
	HRESULT result = device->CreateVertexShader(_shader_buffer->GetBufferPointer(), _shader_buffer->GetBufferSize(), NULL, &_shader_vertex);

	_type = SHADER_VERTEX;

	return SUCCEEDED(result);
}

bool ShaderD3D::initPixelSource(IRenderDevice& rd, const GleamAString& source)
{
	assert(rd.isD3D() && source.size());

	_shader_buffer = compileShader(source.getBuffer(), source.size(), "PixelMain", "ps_5_0");

	if (!_shader_buffer) {
		return false;
	}

	ID3D11Device* device = ((RenderDeviceD3D&)rd).getActiveDevice();
	HRESULT result = device->CreatePixelShader(_shader_buffer->GetBufferPointer(), _shader_buffer->GetBufferSize(), NULL, &_shader_pixel);

	_type = SHADER_PIXEL;

	return SUCCEEDED(result);
}

bool ShaderD3D::initDomainSource(IRenderDevice& rd, const GleamAString& source)
{
	assert(rd.isD3D() && source.size());

	_shader_buffer = compileShader(source.getBuffer(), source.size(), "DomainMain", "ds_5_0");

	if (!_shader_buffer) {
		return false;
	}

	ID3D11Device* device = ((RenderDeviceD3D&)rd).getActiveDevice();
	HRESULT result = device->CreateDomainShader(_shader_buffer->GetBufferPointer(), _shader_buffer->GetBufferSize(), NULL, &_shader_domain);

	_type = SHADER_DOMAIN;

	return SUCCEEDED(result);
}

bool ShaderD3D::initGeometrySource(IRenderDevice& rd, const GleamAString& source)
{
	assert(rd.isD3D() && source.size());

	_shader_buffer = compileShader(source.getBuffer(), source.size(), "GeometryMain", "gs_5_0");

	if (!_shader_buffer) {
		return false;
	}

	ID3D11Device* device = ((RenderDeviceD3D&)rd).getActiveDevice();
	HRESULT result = device->CreateGeometryShader(_shader_buffer->GetBufferPointer(), _shader_buffer->GetBufferSize(), NULL, &_shader_geometry);

	_type = SHADER_GEOMETRY;

	return SUCCEEDED(result);
}

bool ShaderD3D::initHullSource(IRenderDevice& rd, const GleamAString& source)
{
	assert(rd.isD3D() && source.size());

	_shader_buffer = compileShader(source.getBuffer(), source.size(), "HullMain", "hs_5_0");

	if (!_shader_buffer) {
		return false;
	}

	ID3D11Device* device = ((RenderDeviceD3D&)rd).getActiveDevice();
	HRESULT result = device->CreateHullShader(_shader_buffer->GetBufferPointer(), _shader_buffer->GetBufferSize(), NULL, &_shader_hull);

	_type = SHADER_HULL;

	return SUCCEEDED(result);
}

bool ShaderD3D::initComputeSource(IRenderDevice& rd, const GleamAString& source)
{
	assert(rd.isD3D() && source.size());

	_shader_buffer = compileShader(source.getBuffer(), source.size(), "ComputeMain", "cs_5_0");

	if (!_shader_buffer) {
		return false;
	}

	ID3D11Device* device = ((RenderDeviceD3D&)rd).getActiveDevice();
	HRESULT result = device->CreateComputeShader(_shader_buffer->GetBufferPointer(), _shader_buffer->GetBufferSize(), NULL, &_shader_compute);

	_type = SHADER_COMPUTE;

	return SUCCEEDED(result);
}

bool ShaderD3D::initVertexSource(IRenderDevice& rd, const char* source)
{
	assert(rd.isD3D() && source);

	_shader_buffer = compileShader(source, strlen(source), "VertexMain", "vs_5_0");

	if (!_shader_buffer) {
		return false;
	}

	ID3D11Device* device = ((RenderDeviceD3D&)rd).getActiveDevice();
	HRESULT result = device->CreateVertexShader(_shader_buffer->GetBufferPointer(), _shader_buffer->GetBufferSize(), NULL, &_shader_vertex);

	_type = SHADER_VERTEX;

	return SUCCEEDED(result);
}

bool ShaderD3D::initPixelSource(IRenderDevice& rd, const char* source)
{
	assert(rd.isD3D() && source);

	_shader_buffer = compileShader(source, strlen(source), "PixelMain", "ps_5_0");

	if (!_shader_buffer) {
		return false;
	}

	ID3D11Device* device = ((RenderDeviceD3D&)rd).getActiveDevice();
	HRESULT result = device->CreatePixelShader(_shader_buffer->GetBufferPointer(), _shader_buffer->GetBufferSize(), NULL, &_shader_pixel);

	_type = SHADER_PIXEL;

	return SUCCEEDED(result);
}

bool ShaderD3D::initDomainSource(IRenderDevice& rd, const char* source)
{
	assert(rd.isD3D() && source);

	_shader_buffer = compileShader(source, strlen(source), "DomainMain", "ds_5_0");

	if (!_shader_buffer) {
		return false;
	}

	ID3D11Device* device = ((RenderDeviceD3D&)rd).getActiveDevice();
	HRESULT result = device->CreateDomainShader(_shader_buffer->GetBufferPointer(), _shader_buffer->GetBufferSize(), NULL, &_shader_domain);

	_type = SHADER_DOMAIN;

	return SUCCEEDED(result);
}

bool ShaderD3D::initGeometrySource(IRenderDevice& rd, const char* source)
{
	assert(rd.isD3D() && source);

	_shader_buffer = compileShader(source, strlen(source), "GeometryMain", "gs_5_0");

	if (!_shader_buffer) {
		return false;
	}

	ID3D11Device* device = ((RenderDeviceD3D&)rd).getActiveDevice();
	HRESULT result = device->CreateGeometryShader(_shader_buffer->GetBufferPointer(), _shader_buffer->GetBufferSize(), NULL, &_shader_geometry);

	_type = SHADER_GEOMETRY;

	return SUCCEEDED(result);
}

bool ShaderD3D::initHullSource(IRenderDevice& rd, const char* source)
{
	assert(rd.isD3D() && source);

	_shader_buffer = compileShader(source, strlen(source), "HullMain", "hs_5_0");

	if (!_shader_buffer) {
		return false;
	}

	ID3D11Device* device = ((RenderDeviceD3D&)rd).getActiveDevice();
	HRESULT result = device->CreateHullShader(_shader_buffer->GetBufferPointer(), _shader_buffer->GetBufferSize(), NULL, &_shader_hull);

	_type = SHADER_HULL;

	return SUCCEEDED(result);
}

bool ShaderD3D::initComputeSource(IRenderDevice& rd, const char* source)
{
	assert(rd.isD3D() && source);

	_shader_buffer = compileShader(source, strlen(source), "ComputeMain", "cs_5_0");

	if (!_shader_buffer) {
		return false;
	}

	ID3D11Device* device = ((RenderDeviceD3D&)rd).getActiveDevice();
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

bool ShaderD3D::isD3D(void) const
{
	return true;
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

bool ShaderD3D::loadFile(const GChar* file_path, char*& shader_src, SIZE_T& shader_size) const
{
	assert(file_path);

	Gaff::File shader(file_path, Gaff::File::READ_BINARY);

	if (!shader.isOpen()) {
		GleamAString msg("Failed to open shader file: ");

		#ifdef _UNICODE
			// convert file_path to ASCII
		#else
			msg += file_path;
		#endif

		WriteMessageToLog(msg.getBuffer(), msg.size(), LOG_ERROR);
		return false;
	}

	long size = shader.getFileSize();

	shader_size = size;
	shader_src = (char*)GleamAllocate(sizeof(char) * (unsigned int)shader_size);

	if (!shader_src || size == -1) {
		return false;
	}

	if (!shader.readEntireFile(shader_src)) {
		GleamFree(shader_src);
		shader_src = nullptr;
		shader_size = 0;

		GleamAString msg("Failed to read shader file: ");

		#ifdef _UNICODE
			// convert file_path to ASCII
		#else
			msg += file_path;
		#endif

		WriteMessageToLog(msg.getBuffer(), msg.size(), LOG_ERROR);
		return false;
	}

	return true;
}

ID3DBlob* ShaderD3D::compileShader(const char* shader_src, SIZE_T shader_size, /*macro, include,*/ LPCSTR entry_point, LPCSTR target)
{
	assert(shader_src && shader_size && entry_point && target);

	ID3DBlob* shader_buffer;
	ID3DBlob* error_buffer;

	HRESULT result = D3DCompile(shader_src, shader_size, NULL, NULL /*macros*/, NULL /*include*/,
								entry_point, target, SHADER_FLAGS, 0, &shader_buffer, &error_buffer);

	if (FAILED(result)) {
		if (error_buffer) {
			const char* error_msg = (const char*)error_buffer->GetBufferPointer();
			SIZE_T error_size = error_buffer->GetBufferSize();

			WriteMessageToLog(error_msg, error_size, LOG_ERROR);
			error_buffer->Release();
		}

		return nullptr;
	}

	return shader_buffer;
}

NS_END
