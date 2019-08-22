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

#include "Gleam_MeshBase.h"
#include "Gleam_IRenderDevice.h"
#include "Gleam_IBuffer.h"

NS_GLEAM

MeshBase::MeshBase(void):
	_indices(nullptr), _index_count(0)
{
}

MeshBase::~MeshBase(void)
{
}

void MeshBase::addBuffer(IBuffer* buffer, uint32_t offset)
{
	_vert_data.emplace_back(buffer);
	_offsets.emplace_back(offset);
}

const IBuffer* MeshBase::getBuffer(int32_t index) const
{
	GAFF_ASSERT(index < static_cast<int32_t>(_vert_data.size()));
	return _vert_data[index];
}

IBuffer* MeshBase::getBuffer(int32_t index)
{
	GAFF_ASSERT(index < static_cast<int32_t>(_vert_data.size()));
	return _vert_data[index];
}

int32_t MeshBase::getBufferCount(void) const
{
	return static_cast<int32_t>(_vert_data.size());
}

uint32_t MeshBase::getBufferOffset(int32_t index) const
{
	return _offsets[index];
}

void MeshBase::setIndiceBuffer(IBuffer* buffer)
{
	GAFF_ASSERT(buffer);
	_indices = buffer;
}

const IBuffer* MeshBase::getIndiceBuffer(void) const
{
	return _indices;
}

IBuffer* MeshBase::getIndiceBuffer(void)
{
	return _indices;
}

MeshBase::TopologyType MeshBase::getTopologyType(void) const
{
	return _topology;
}

void MeshBase::setIndexCount(int32_t count)
{
	_index_count = count;
}

int32_t MeshBase::getIndexCount(void) const
{
	return _index_count;
}

bool MeshBase::addVertDataHelper(
	IRenderDevice& rd, const void* vert_data, int32_t vert_count, int32_t vert_size,
	int32_t* indices, int32_t index_count, TopologyType primitive_type,
	IBuffer* index_buffer, IBuffer* vert_buffer
)
{
	GAFF_ASSERT(vert_data && vert_count && vert_size && indices && index_count &&
			index_buffer && vert_buffer);

	IBuffer::BufferSettings vert_settings
	{
		vert_data,
		static_cast<size_t>(vert_count) * vert_size,
		vert_size,
		IBuffer::BT_VERTEX_DATA,
		IBuffer::MT_NONE,
		true
	};

	if (!vert_buffer->init(rd, vert_settings)) {
		return false;
	}

	IBuffer::BufferSettings index_settings
	{
		indices,
		sizeof(uint32_t) * index_count,
		sizeof(uint32_t),
		IBuffer::BT_INDEX_DATA,
		IBuffer::MT_NONE,
		true
	};

	if (!index_buffer->init(rd, index_settings)) {
		return false;
	}

	addBuffer(vert_buffer);
	setIndiceBuffer(index_buffer);
	_index_count = index_count;

	setTopologyType(primitive_type);

	return true;
}

NS_END
