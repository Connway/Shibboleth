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

#include "Gleam_MeshBase.h"
#include "Gleam_IRenderDevice.h"
#include "Gleam_Global.h"
#include "Gleam_IBuffer.h"
#include <Gaff_IncludeAssert.h>

NS_GLEAM

MeshBase::MeshBase(void):
	_indices(nullptr), _index_count(0)
{
}

MeshBase::~MeshBase(void)
{
}

void MeshBase::destroy(void)
{
	for (unsigned int i = 0; i < _vert_data.size(); ++i) {
		_vert_data[i]->release();
	}

	SAFEGAFFRELEASE(_indices);
	_index_count = 0;

	_vert_data.clear();
}

void MeshBase::addBuffer(IBuffer* buffer)
{
	_vert_data.push(buffer);
	buffer->addRef();
}

const IBuffer* MeshBase::getBuffer(unsigned int index) const
{
	assert(index < _vert_data.size());
	return _vert_data[index];
}

IBuffer* MeshBase::getBuffer(unsigned int index)
{
	assert(index < _vert_data.size());
	return _vert_data[index];
}

unsigned int MeshBase::getBufferCount(void) const
{
	return _vert_data.size();
}

void MeshBase::setIndiceBuffer(IBuffer* buffer)
{
	assert(buffer);
	SAFEGAFFRELEASE(_indices);
	_indices = buffer;
	buffer->addRef();
}

const IBuffer* MeshBase::getIndiceBuffer(void) const
{
	return _indices;
}

IBuffer* MeshBase::getIndiceBuffer(void)
{
	return _indices;
}

MeshBase::TOPOLOGY_TYPE MeshBase::getTopologyType(void) const
{
	return _topology;
}

void MeshBase::setIndexCount(unsigned int count)
{
	_index_count = count;
}

unsigned int MeshBase::getIndexCount(void) const
{
	return _index_count;
}

bool MeshBase::addVertDataHelper(
	IRenderDevice& rd, const void* vert_data, unsigned int vert_count, unsigned int vert_size,
	unsigned int* indices, unsigned int index_count, TOPOLOGY_TYPE primitive_type,
	IBuffer* index_buffer, IBuffer* vert_buffer
)
{
	assert(vert_data && vert_count && vert_size && indices && index_count &&
			index_buffer && vert_buffer);

	if (!vert_buffer->init(rd, vert_data, vert_count * vert_size, IBuffer::VERTEX_DATA, vert_size)) {
		return false;
	}

	if (!index_buffer->init(rd, indices, sizeof(unsigned int) * index_count, IBuffer::INDEX_DATA, sizeof(unsigned int))) {
		return false;
	}

	addBuffer(vert_buffer);
	setIndiceBuffer(index_buffer);
	_index_count = index_count;

	setTopologyType(primitive_type);

	return true;
}

NS_END
