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

#pragma once

#include "Gleam_Defines.h"

NS_GLEAM

class IRenderDevice;
class IBuffer;

class IMesh
{
public:
	enum TopologyType {
		POINT_LIST = 0,
		LINE_LIST,
		LINE_STRIP,
		TRIANGLE_LIST,
		TRIANGLE_STRIP,

		LINE_LIST_ADJ,
		LINE_STRIP_ADJ,
		TRIANGLE_LIST_ADJ,
		TRIANGLE_STRIP_ADJ,

		TOPOLOGY_SIZE
	};

	IMesh(void) {}
	virtual ~IMesh(void) {}

	template <class Vertex>
	bool addVertData(
		IRenderDevice& rd, const Vertex* vert_data, int32_t vert_count,
		int32_t* indices, int32_t index_count, TopologyType primitive_type = TRIANGLE_LIST)
	{
		return addVertData(rd, vert_data, vert_count, sizeof(Vertex), indices, index_count, primitive_type);
	}

	virtual bool addVertData(
		IRenderDevice& rd, const void* vert_data, int32_t vert_count, int32_t vert_size,
		int32_t* indices, int32_t index_count, TopologyType primitive_type = TRIANGLE_LIST
	) = 0;

	virtual void addBuffer(IBuffer* buffer, uint32_t offset = 0) = 0;
	virtual const IBuffer* getBuffer(int32_t index) const = 0;
	virtual IBuffer* getBuffer(int32_t index) = 0;
	virtual int32_t getBufferCount(void) const = 0;
	virtual uint32_t getBufferOffset(int32_t index) const = 0;

	virtual void setIndiceBuffer(IBuffer* buffer) = 0;
	virtual const IBuffer* getIndiceBuffer(void) const = 0;
	virtual IBuffer* getIndiceBuffer(void) = 0;

	virtual void setTopologyType(TopologyType topology) = 0;
	virtual TopologyType getTopologyType(void) const = 0;

	virtual void setIndexCount(int32_t count) = 0;
	virtual int32_t getIndexCount(void) const = 0;

	virtual void renderNonIndexed(IRenderDevice& rd, int32_t vert_count, int32_t start_location = 0) = 0;
	virtual void renderInstanced(IRenderDevice& rd, int32_t count) = 0;
	virtual void render(IRenderDevice& rd) = 0;

	virtual RendererType getRendererType(void) const = 0;

	GAFF_NO_COPY(IMesh);
};

NS_END
