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

#ifdef GLEAM_USE_D3D11

#include "Gleam_Buffer_Direct3D11.h"
#include "Gleam_RenderDevice_Direct3D11.h"
#include "Gleam_IRenderDevice.h"
#include "Gleam_IncludeD3D11.h"

NS_GLEAM

static const D3D11_BIND_FLAG g_type_map[static_cast<size_t>(IBuffer::Type::Count)] = {
	D3D11_BIND_VERTEX_BUFFER,
	D3D11_BIND_INDEX_BUFFER,
	D3D11_BIND_CONSTANT_BUFFER,
	D3D11_BIND_SHADER_RESOURCE
};

static const D3D11_MAP g_map_map[static_cast<size_t>(IBuffer::MapType::Count)] = {
	D3D11_MAP_READ,
	D3D11_MAP_READ,
	D3D11_MAP_WRITE_DISCARD,
	D3D11_MAP_WRITE_NO_OVERWRITE,
	D3D11_MAP_READ_WRITE
};

Buffer::Buffer(void):
	_buffer(nullptr)
{
}

Buffer::~Buffer(void)
{
	destroy();
}

bool Buffer::init(IRenderDevice& rd, const Settings& buffer_settings)
{
	GAFF_ASSERT(rd.getRendererType() == RendererType::Direct3D11 && !_buffer);

	RenderDevice& rd3d = static_cast<RenderDevice&>(rd);
	ID3D11Device5* const device = rd3d.getDevice();

	D3D11_BUFFER_DESC desc;

	desc.BindFlags = g_type_map[static_cast<size_t>(buffer_settings.type)];
	desc.ByteWidth = static_cast<UINT>(buffer_settings.size);
	desc.MiscFlags = (buffer_settings.type == Type::StructuredData) ? D3D11_RESOURCE_MISC_BUFFER_STRUCTURED : 0;
	desc.StructureByteStride = (buffer_settings.type == Type::StructuredData) ? static_cast<UINT>(buffer_settings.stride) : 0;
	desc.CPUAccessFlags = 0;

	const bool cpu_read = (buffer_settings.cpu_access == MapType::Read) || (buffer_settings.cpu_access == MapType::ReadWrite);
	const bool cpu_write = (buffer_settings.cpu_access == MapType::ReadWrite) ||
		(buffer_settings.cpu_access == MapType::Write) ||
		(buffer_settings.cpu_access == MapType::WriteNoOverwrite);

	if (buffer_settings.gpu_read_only) {
		if (cpu_read) {
			desc.Usage = D3D11_USAGE_STAGING;
		} else if (cpu_write) {
			desc.Usage = D3D11_USAGE_DYNAMIC;
		} else {
			desc.Usage = D3D11_USAGE_IMMUTABLE;
		}
	} else {
		if (cpu_read || cpu_write) {
			desc.Usage = D3D11_USAGE_STAGING;
		} else {
			desc.Usage = D3D11_USAGE_DEFAULT;
		}
	}

	if (cpu_read) {
		desc.CPUAccessFlags |= D3D11_CPU_ACCESS_READ;
	}

	if (cpu_write) {
		desc.CPUAccessFlags |= D3D11_CPU_ACCESS_WRITE;
	}

	_buffer_type = buffer_settings.type;
	_elem_size = buffer_settings.element_size;
	_stride = buffer_settings.stride;
	_size = buffer_settings.size;

	if (buffer_settings.data) {
		D3D11_SUBRESOURCE_DATA subres_data;
		subres_data.pSysMem = buffer_settings.data;
		subres_data.SysMemPitch = 0;
		subres_data.SysMemSlicePitch = 0;

		return SUCCEEDED(device->CreateBuffer(&desc, &subres_data, &_buffer));
	}

	return SUCCEEDED(device->CreateBuffer(&desc, nullptr, &_buffer));
}

void Buffer::destroy(void)
{
	GAFF_COM_SAFE_RELEASE(_buffer)
}

bool Buffer::update(IRenderDevice& rd, const void* data, size_t size, size_t offset)
{
	GAFF_ASSERT(rd.getRendererType() == RendererType::Direct3D11 && data && size);
	RenderDevice& rd3d = static_cast<RenderDevice&>(rd);
	ID3D11DeviceContext3* const context = rd3d.getDeviceContext();
	D3D11_MAPPED_SUBRESOURCE mapped_resource;

	const HRESULT result = context->Map(_buffer, 0, (offset) ? D3D11_MAP_WRITE_NO_OVERWRITE : D3D11_MAP_WRITE_DISCARD, 0, &mapped_resource);

	if (FAILED(result)) {
		return false;
	}

	memcpy(reinterpret_cast<int8_t*>(mapped_resource.pData) + offset, data, size);
	context->Unmap(_buffer, 0);

	return true;
}

void* Buffer::map(IRenderDevice& rd, MapType map_type)
{
	GAFF_ASSERT((rd.getRendererType() == RendererType::Direct3D11) && (map_type != MapType::None));
	RenderDevice& rd3d = static_cast<RenderDevice&>(rd);
	ID3D11DeviceContext3* const context = rd3d.getDeviceContext();
	D3D11_MAPPED_SUBRESOURCE mapped_resource;

	const HRESULT result = context->Map(_buffer, 0, g_map_map[static_cast<size_t>(map_type)], 0, &mapped_resource);
	return (FAILED(result)) ? nullptr : mapped_resource.pData;
}

void Buffer::unmap(IRenderDevice& rd)
{
	GAFF_ASSERT(rd.getRendererType() == RendererType::Direct3D11);
	RenderDevice& rd3d = static_cast<RenderDevice&>(rd);
	ID3D11DeviceContext3* const context = rd3d.getDeviceContext();
	context->Unmap(_buffer, 0);
}

RendererType Buffer::getRendererType(void) const
{
	return RendererType::Direct3D11;
}

ID3D11Buffer* Buffer::getBuffer(void) const
{
	return _buffer;
}

NS_END

#endif
