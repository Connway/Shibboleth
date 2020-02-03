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

#include "Gleam_Mesh_Direct3D11.h"
#include "Gleam_RenderDevice_Direct3D11.h"
#include "Gleam_Buffer_Direct3D11.h"
#include "Gleam_IRenderDevice.h"

NS_GLEAM

static D3D11_PRIMITIVE_TOPOLOGY g_topology_map[IMesh::TOPOLOGY_SIZE] = {
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

MeshD3D11::MeshD3D11(void)
{
}

MeshD3D11::~MeshD3D11(void)
{
}

bool MeshD3D11::addVertData(
	IRenderDevice& rd, const void* vert_data, int32_t vert_count, int32_t vert_size,
	int32_t* indices, int32_t index_count, TopologyType primitive_type
)
{
	IBuffer* index_buffer = GLEAM_ALLOCT(BufferD3D11);
	IBuffer* vert_buffer = GLEAM_ALLOCT(BufferD3D11);

	if (!index_buffer || !vert_buffer) {
		if (index_buffer) {
			GAFF_FREET(index_buffer, *GetAllocator());
		}

		if (vert_buffer) {
			GAFF_FREET(vert_buffer, *GetAllocator());
		}

		return false;
	}

	bool ret = addVertDataHelper(
		rd, vert_data, vert_count, vert_size, indices, index_count,
		primitive_type, index_buffer, vert_buffer
	);

	if (!ret) {
		GAFF_FREET(index_buffer, *GetAllocator());
		GAFF_FREET(vert_buffer, *GetAllocator());
	}

	cacheBuffers();
	return ret;
}

void MeshD3D11::addBuffer(IBuffer* buffer, uint32_t offset)
{
	MeshBase::addBuffer(buffer, offset);
	cacheBuffers();
}

void MeshD3D11::setTopologyType(TopologyType topology)
{
	_d3d_topology = g_topology_map[topology];
	_topology = topology;
}

void MeshD3D11::renderNonIndexed(IRenderDevice& rd, int32_t vert_count, int32_t start_location)
{
	GAFF_ASSERT(rd.getRendererType() == RendererType::DIRECT3D11 && _vert_data.size() && _indices && _indices->getRendererType() == RendererType::DIRECT3D11);

	RenderDeviceD3D11& rd3d = static_cast<RenderDeviceD3D11&>(rd);
	ID3D11DeviceContext3* const context = rd3d.getDeviceContext();

	context->IASetVertexBuffers(0, static_cast<UINT>(_buffers.size()), _buffers.data(), _strides.data(), _offsets.data());
	context->IASetPrimitiveTopology(_d3d_topology);
	context->Draw(vert_count, start_location);
}

void MeshD3D11::renderInstanced(IRenderDevice& rd, int32_t count)
{
	GAFF_ASSERT(rd.getRendererType() == RendererType::DIRECT3D11 && _vert_data.size() && _indices && _indices->getRendererType() == RendererType::DIRECT3D11);

	RenderDeviceD3D11& rd3d = static_cast<RenderDeviceD3D11&>(rd);
	ID3D11DeviceContext3* const context = rd3d.getDeviceContext();

	context->IASetVertexBuffers(0, static_cast<UINT>(_buffers.size()), _buffers.data(), _strides.data(), _offsets.data());
	context->IASetIndexBuffer(static_cast<BufferD3D11*>(_indices)->getBuffer(), DXGI_FORMAT_R32_UINT, 0);
	context->IASetPrimitiveTopology(_d3d_topology);
	context->DrawIndexedInstanced(getIndexCount(), count, 0, 0, 0);
}

void MeshD3D11::render(IRenderDevice& rd)
{
	GAFF_ASSERT(rd.getRendererType() == RendererType::DIRECT3D11 && _vert_data.size() && _indices && _indices->getRendererType() == RendererType::DIRECT3D11);

	RenderDeviceD3D11& rd3d = static_cast<RenderDeviceD3D11&>(rd);
	ID3D11DeviceContext3* const context = rd3d.getDeviceContext();

	context->IASetVertexBuffers(0, static_cast<UINT>(_buffers.size()), _buffers.data(), _strides.data(), _offsets.data());
	context->IASetIndexBuffer(static_cast<BufferD3D11*>(_indices)->getBuffer(), DXGI_FORMAT_R32_UINT, 0);
	context->IASetPrimitiveTopology(_d3d_topology);
	context->DrawIndexed(getIndexCount(), 0, 0);
}

RendererType MeshD3D11::getRendererType(void) const
{
	return RendererType::DIRECT3D11;
}

void MeshD3D11::cacheBuffers(void)
{
	_buffers.clear();
	_strides.clear();

	IBuffer* temp = nullptr;

	for (int32_t i = 0; i < static_cast<int32_t>(_vert_data.size()); ++i) {
		temp = _vert_data[i];
		GAFF_ASSERT(temp && temp->getRendererType() == RendererType::DIRECT3D11);
		_buffers.emplace_back(static_cast<BufferD3D11*>(temp)->getBuffer());
		_strides.emplace_back(temp->getStride());
	}
}

NS_END

#endif
