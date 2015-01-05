/************************************************************************************
Copyright (C) 2015 by Nicholas LaCroix

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
#include "Gleam_RenderDevice_Direct3D.h"
#include "Gleam_IncludeD3D11.h"
#include <Gaff_IncludeAssert.h>

NS_GLEAM

static D3D11_BIND_FLAG _type_map[IBuffer::BUFFER_TYPE_SIZE] = {
	D3D11_BIND_VERTEX_BUFFER,
	D3D11_BIND_INDEX_BUFFER,
	D3D11_BIND_CONSTANT_BUFFER
};

static D3D11_MAP _map_map[IBuffer::MAP_TYPE_SIZE] = {
	D3D11_MAP_READ,
	D3D11_MAP_READ,
	D3D11_MAP_WRITE_DISCARD,
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

bool BufferD3D::init(IRenderDevice& rd, const void* data, unsigned int size, BUFFER_TYPE buffer_type, unsigned int stride, MAP_TYPE cpu_access)
{
	assert(rd.isD3D() && !_buffer);
	ID3D11Device* device = ((RenderDeviceD3D&)rd).getActiveDevice();

	D3D11_SUBRESOURCE_DATA subres_data;
	D3D11_BUFFER_DESC desc;

	desc.BindFlags = _type_map[buffer_type];
	desc.ByteWidth = size;
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = 0;
	desc.StructureByteStride = 0;
	desc.Usage = (cpu_access != NONE) ? D3D11_USAGE_DYNAMIC : D3D11_USAGE_DEFAULT;

	switch (cpu_access) {
		case READ:
			desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
			break;

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
	_stride = stride;
	_size = size;

	return SUCCEEDED(device->CreateBuffer(&desc, (data) ? &subres_data : nullptr, &_buffer));
}

void BufferD3D::destroy(void)
{
	SAFERELEASE(_buffer)
}

bool BufferD3D::update(IRenderDevice& rd, const void* data, unsigned int size)
{
	assert(rd.isD3D() && data);
	ID3D11DeviceContext* context = ((RenderDeviceD3D&)rd).getActiveDeviceContext();
	D3D11_MAPPED_SUBRESOURCE mapped_resource;

	HRESULT result = context->Map(_buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped_resource);
	RETURNIFFAILED(result)

	memcpy(mapped_resource.pData, data, size);
	context->Unmap(_buffer, 0);

	return true;
}

void* BufferD3D::map(IRenderDevice& rd, MAP_TYPE map_type)
{
	assert(rd.isD3D() && map_type != NONE);
	ID3D11DeviceContext* context = ((RenderDeviceD3D&)rd).getActiveDeviceContext();
	D3D11_MAPPED_SUBRESOURCE mapped_resource;

	HRESULT result = context->Map(_buffer, 0, _map_map[map_type], 0, &mapped_resource);
	return (FAILED(result)) ? nullptr : mapped_resource.pData;
}

void BufferD3D::unmap(IRenderDevice& rd)
{
	assert(rd.isD3D());
	ID3D11DeviceContext* context = ((RenderDeviceD3D&)rd).getActiveDeviceContext();
	context->Unmap(_buffer, 0);
}

bool BufferD3D::isD3D(void) const
{
	return true;
}

ID3D11Buffer* BufferD3D::getBuffer(void) const
{
	return _buffer;
}

NS_END

#endif