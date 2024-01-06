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

#ifdef GLEAM_USE_D3D11

#include "Gleam_Mesh_Direct3D11.h"
#include "Gleam_RenderDevice_Direct3D11.h"
#include "Gleam_Buffer_Direct3D11.h"

NS_GLEAM

static constexpr D3D11_PRIMITIVE_TOPOLOGY g_topology_map[static_cast<size_t>(IMesh::Topology::Count)] = {
	D3D11_PRIMITIVE_TOPOLOGY_POINTLIST,
	D3D11_PRIMITIVE_TOPOLOGY_LINELIST,
	D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP,
	D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST,
	D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP,

	D3D11_PRIMITIVE_TOPOLOGY_LINELIST_ADJ,
	D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP_ADJ,
	D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST_ADJ,
	D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP_ADJ
};

Mesh::Mesh(void)
{
}

Mesh::~Mesh(void)
{
}

void Mesh::clear(void)
{
	_buffers.clear();
	_strides.clear();
	_vert_data.clear();
	_offsets.clear();
	_indices = nullptr;
}

void Mesh::addBuffer(IBuffer* buffer, uint32_t offset)
{
	GAFF_ASSERT(buffer->getRendererType() == RendererType::Direct3D11);

	_vert_data.emplace_back(static_cast<Buffer*>(buffer));
	_offsets.emplace_back(offset);

	_vert_data.shrink_to_fit();
	_offsets.shrink_to_fit();

	cacheBuffers();
}

const IBuffer* Mesh::getBuffer(int32_t index) const
{
	GAFF_ASSERT(index >= 0 && index < static_cast<int32_t>(_vert_data.size()));
	return _vert_data[index];
}

IBuffer* Mesh::getBuffer(int32_t index)
{
	GAFF_ASSERT(index >= 0 && index < static_cast<int32_t>(_vert_data.size()));
	return _vert_data[index];
}

int32_t Mesh::getBufferCount(void) const
{
	return static_cast<int32_t>(_vert_data.size());
}

uint32_t Mesh::getBufferOffset(int32_t index) const
{
	return _offsets[index];
}

void Mesh::setIndiceBuffer(IBuffer* buffer)
{
	GAFF_ASSERT(buffer->getRendererType() == RendererType::Direct3D11);

	_indices = static_cast<Buffer*>(buffer);

	_indice_format = DXGI_FORMAT_R16_UINT;

	if (_indices) {
		switch (_indices->getElementSize()) {
			case 4:
				_indice_format = DXGI_FORMAT_R32_UINT;
				break;

			case 2:
				_indice_format = DXGI_FORMAT_R16_UINT;
				break;

			case 1:
				_indice_format = DXGI_FORMAT_R8_UINT;
				break;

			default:
				GAFF_ASSERT(false);
		}
	}
}

const IBuffer* Mesh::getIndiceBuffer(void) const
{
	return _indices;
}

IBuffer* Mesh::getIndiceBuffer(void)
{
	return _indices;
}

void Mesh::renderNonIndexed(IRenderDevice& rd, int32_t vert_count, int32_t vert_offset)
{
	GAFF_ASSERT(rd.getRendererType() == RendererType::Direct3D11);
	GAFF_ASSERT(_vert_data.size());
	GAFF_ASSERT(_indices);

	RenderDevice& rd3d = static_cast<RenderDevice&>(rd);
	ID3D11DeviceContext3* const context = rd3d.getDeviceContext();

	context->IASetVertexBuffers(0, static_cast<UINT>(_buffers.size()), _buffers.data(), _strides.data(), _offsets.data());
	context->IASetPrimitiveTopology(getD3DTopology());
	context->Draw(vert_count, vert_offset);
}

void Mesh::renderInstanced(IRenderDevice& rd, int32_t instance_count, int32_t index_offset, int32_t vert_offset, int32_t instance_offset)
{
	GAFF_ASSERT(rd.getRendererType() == RendererType::Direct3D11);
	GAFF_ASSERT(_vert_data.size());
	GAFF_ASSERT(_indices);

	RenderDevice& rd3d = static_cast<RenderDevice&>(rd);
	ID3D11DeviceContext3* const context = rd3d.getDeviceContext();

	context->IASetVertexBuffers(0, static_cast<UINT>(_buffers.size()), _buffers.data(), _strides.data(), _offsets.data());
	context->IASetIndexBuffer(static_cast<Buffer*>(_indices)->getBuffer(), _indice_format, 0);
	context->IASetPrimitiveTopology(getD3DTopology());
	context->DrawIndexedInstanced(getIndexCount(), instance_count, index_offset, vert_offset, instance_offset);
}

void Mesh::render(IRenderDevice& rd, int32_t index_offset, int32_t vert_offset)
{
	GAFF_ASSERT(rd.getRendererType() == RendererType::Direct3D11);
	GAFF_ASSERT(_vert_data.size());
	GAFF_ASSERT(_indices);

	RenderDevice& rd3d = static_cast<RenderDevice&>(rd);
	ID3D11DeviceContext3* const context = rd3d.getDeviceContext();

	context->IASetVertexBuffers(0, static_cast<UINT>(_buffers.size()), _buffers.data(), _strides.data(), _offsets.data());
	context->IASetIndexBuffer(static_cast<Buffer*>(_indices)->getBuffer(), _indice_format, 0);
	context->IASetPrimitiveTopology(getD3DTopology());
	context->DrawIndexed(getIndexCount(), index_offset, vert_offset);
}

RendererType Mesh::getRendererType(void) const
{
	return RendererType::Direct3D11;
}

D3D11_PRIMITIVE_TOPOLOGY Mesh::getD3DTopology(void) const
{
	return g_topology_map[static_cast<size_t>(getTopology())];
}

void Mesh::cacheBuffers(void)
{
	_buffers.clear();
	_strides.clear();

	_buffers.reserve(_vert_data.size());
	_strides.reserve(_vert_data.size());

	for (int32_t i = 0; i < static_cast<int32_t>(_vert_data.size()); ++i) {
		Buffer* buffer = _vert_data[i];

		_buffers.emplace_back(buffer->getBuffer());
		_strides.emplace_back(buffer->getStride());
	}

	_buffers.shrink_to_fit();
	_strides.shrink_to_fit();
}

NS_END

#endif
