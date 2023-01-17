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

#include "Gleam_MeshBase.h"
#include "Gleam_IRenderDevice.h"
#include "Gleam_IBuffer.h"

NS_GLEAM

void MeshBase::clear(void)
{
	_vert_data.clear();
	_offsets.clear();
	_indices = nullptr;
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

NS_END
