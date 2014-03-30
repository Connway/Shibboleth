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

#if defined(_WIN32) || defined(_WIN64)

#include "Gleam_Mesh_Direct3D.h"
#include "Gleam_RenderDevice_Direct3D.h"
#include "Gleam_Buffer_Direct3D.h"
#include <Gaff_IncludeAssert.h>

NS_GLEAM

static D3D11_PRIMITIVE_TOPOLOGY _topology_map[IMesh::TOPOLOGY_SIZE] = {
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

MeshD3D::MeshD3D(void)
{
}

MeshD3D::~MeshD3D(void)
{
	destroy();
}

bool MeshD3D::addVertData(
	IRenderDevice& rd, const void* vert_data, unsigned int vert_count, unsigned int vert_size,
	unsigned int* indices, unsigned int index_count, TOPOLOGY_TYPE primitive_type
)
{
	IBuffer* index_buffer = GleamAllocateT(BufferD3D);
	IBuffer* vert_buffer = GleamAllocateT(BufferD3D);

	if (!index_buffer || !vert_buffer) {
		if (index_buffer) {
			GleamFree(index_buffer);
		}

		return false;
	}

	bool ret = addVertDataHelper(
		rd, vert_data, vert_count, vert_size, indices, index_count,
		primitive_type, index_buffer, vert_buffer
	);

	if (!ret) {
		GleamFree(index_buffer);
		GleamFree(vert_buffer);
	}

	return ret;
}

void MeshD3D::addBuffer(IBuffer* buffer)
{
	IMesh::addBuffer(buffer);
	cacheBuffers();
}

void MeshD3D::setTopologyType(TOPOLOGY_TYPE topology)
{
	_d3d_topology = _topology_map[topology];
	_topology = topology;
}

void MeshD3D::render(IRenderDevice& rd)
{
	assert(rd.isD3D() && _vert_data.size() && _indices && _indices->isD3D());

	ID3D11DeviceContext* context = ((RenderDeviceD3D&)rd).getActiveDeviceContext();
	context->IASetVertexBuffers(0, _buffers.size(), _buffers.getArray(), _strides.getArray(), _offsets.getArray());
	context->IASetIndexBuffer(((BufferD3D*)_indices)->getBuffer(), DXGI_FORMAT_R32_UINT, 0);
	context->IASetPrimitiveTopology(_d3d_topology);
	context->DrawIndexed(getIndexCount(), 0, 0);
}

void MeshD3D::renderInstanced(IRenderDevice& rd, unsigned int count)
{
	assert(rd.isD3D() && _vert_data.size() && _indices && _indices->isD3D());

	ID3D11DeviceContext* context = ((RenderDeviceD3D&)rd).getActiveDeviceContext();
	context->IASetVertexBuffers(0, _buffers.size(), _buffers.getArray(), _strides.getArray(), _offsets.getArray());
	context->IASetIndexBuffer(((BufferD3D*)_indices)->getBuffer(), DXGI_FORMAT_R32_UINT, 0);
	context->IASetPrimitiveTopology(_d3d_topology);
	context->DrawIndexedInstanced(getIndexCount(), count, 0, 0, 0);
}

bool MeshD3D::isD3D(void) const
{
	return true;
}

void MeshD3D::cacheBuffers(void)
{
	_buffers.clear();
	_strides.clear();
	_offsets.clear();

	IBuffer* temp = nullptr;

	for (unsigned int i = 0; i < _vert_data.size(); ++i) {
		temp = _vert_data[i];
		assert(temp && temp->isD3D());
		_buffers.push(((BufferD3D*)temp)->getBuffer());
		_strides.push(temp->getStride());
		_offsets.push(0);
	}
}

NS_END

#endif
