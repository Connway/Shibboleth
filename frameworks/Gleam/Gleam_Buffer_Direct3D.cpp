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

#include "Gleam_Buffer_Direct3D.h"
#include "Gleam_IRenderDevice_Direct3D.h"
#include "Gleam_IRenderDevice.h"
#include "Gleam_IncludeD3D11.h"

NS_GLEAM

static D3D11_BIND_FLAG _type_map[IBuffer::BUFFER_TYPE_SIZE] = {
	D3D11_BIND_VERTEX_BUFFER,
	D3D11_BIND_INDEX_BUFFER,
	D3D11_BIND_CONSTANT_BUFFER,
	D3D11_BIND_SHADER_RESOURCE
};

static D3D11_MAP _map_map[IBuffer::MAP_TYPE_SIZE] = {
	D3D11_MAP_READ,
	D3D11_MAP_READ,
	D3D11_MAP_WRITE_DISCARD,
	D3D11_MAP_WRITE_NO_OVERWRITE,
	D3D11_MAP_READ_WRITE
};

BufferD3D::BufferD3D(void):
	_buffer(nullptr)
{
}

BufferD3D::~BufferD3D(void)
{
	destroy();
}

bool BufferD3D::init(
	IRenderDevice& rd, const void* data, unsigned int size, BUFFER_TYPE buffer_type,
	unsigned int stride, MAP_TYPE cpu_access, bool gpu_read_only, unsigned int structure_byte_stride)
{
	GAFF_ASSERT(rd.getRendererType() == RENDERER_DIRECT3D && !_buffer);
	IRenderDeviceD3D& rd3d = reinterpret_cast<IRenderDeviceD3D&>(*(reinterpret_cast<char*>(&rd) + sizeof(IRenderDevice)));

	ID3D11Device* device = rd3d.getActiveDevice();

	D3D11_SUBRESOURCE_DATA subres_data;
	D3D11_BUFFER_DESC desc;

	desc.BindFlags = _type_map[buffer_type];
	desc.ByteWidth = size;
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = (buffer_type == STRUCTURED_DATA) ? D3D11_RESOURCE_MISC_BUFFER_STRUCTURED : 0;
	desc.StructureByteStride = structure_byte_stride;
	desc.Usage = (gpu_read_only && (cpu_access == WRITE || cpu_access == WRITE_NO_OVERWRITE)) ? D3D11_USAGE_DYNAMIC : D3D11_USAGE_DEFAULT;

	switch (cpu_access) {
		case READ:
			desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
			break;

		case WRITE_NO_OVERWRITE:
		case WRITE:
			desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
			break;

		case READ_WRITE:
			desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ | D3D11_CPU_ACCESS_WRITE;
			break;
	}

	subres_data.pSysMem = data;
	subres_data.SysMemPitch = 0;
	subres_data.SysMemSlicePitch = 0;

	_buffer_type = buffer_type;
	_structure_stride = structure_byte_stride;
	_stride = stride;
	_size = size;

	return SUCCEEDED(device->CreateBuffer(&desc, (data) ? &subres_data : nullptr, &_buffer));
}

void BufferD3D::destroy(void)
{
	SAFERELEASE(_buffer)
}

bool BufferD3D::update(IRenderDevice& rd, const void* data, unsigned int size, unsigned int offset)
{
	GAFF_ASSERT(rd.getRendererType() == RENDERER_DIRECT3D && data);
	IRenderDeviceD3D& rd3d = reinterpret_cast<IRenderDeviceD3D&>(*(reinterpret_cast<char*>(&rd) + sizeof(IRenderDevice)));
	ID3D11DeviceContext* context = rd3d.getActiveDeviceContext();
	D3D11_MAPPED_SUBRESOURCE mapped_resource;

	HRESULT result = context->Map(_buffer, 0, (offset) ? D3D11_MAP_WRITE_NO_OVERWRITE : D3D11_MAP_WRITE_DISCARD, 0, &mapped_resource);
	RETURNIFFAILED(result)

	memcpy(reinterpret_cast<char*>(mapped_resource.pData) + offset, data, size);
	context->Unmap(_buffer, 0);

	return true;
}

void* BufferD3D::map(IRenderDevice& rd, MAP_TYPE map_type)
{
	GAFF_ASSERT(rd.getRendererType() == RENDERER_DIRECT3D && map_type != NONE);
	IRenderDeviceD3D& rd3d = reinterpret_cast<IRenderDeviceD3D&>(*(reinterpret_cast<char*>(&rd) + sizeof(IRenderDevice)));
	ID3D11DeviceContext* context = rd3d.getActiveDeviceContext();
	D3D11_MAPPED_SUBRESOURCE mapped_resource;

	HRESULT result = context->Map(_buffer, 0, _map_map[map_type], 0, &mapped_resource);
	return (FAILED(result)) ? nullptr : mapped_resource.pData;
}

void BufferD3D::unmap(IRenderDevice& rd)
{
	GAFF_ASSERT(rd.getRendererType() == RENDERER_DIRECT3D);
	IRenderDeviceD3D& rd3d = reinterpret_cast<IRenderDeviceD3D&>(*(reinterpret_cast<char*>(&rd) + sizeof(IRenderDevice)));
	ID3D11DeviceContext* context = rd3d.getActiveDeviceContext();
	context->Unmap(_buffer, 0);
}

RendererType BufferD3D::getRendererType(void) const
{
	return RENDERER_DIRECT3D;
}

ID3D11Buffer* BufferD3D::getBuffer(void) const
{
	return _buffer;
}

NS_END

#endif