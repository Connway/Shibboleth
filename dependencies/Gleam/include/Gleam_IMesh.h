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

#pragma once

#include "Gleam_RefCounted.h"
#include "Gleam_String.h"
#include "Gleam_AABB.h"

NS_GLEAM

class IRenderDevice;
class IBuffer;

// Considering only one function is virtual, it should probably be called MeshDataBase?
// But I really don't like that name for some reason ...
class IMesh : public GleamRefCounted
{
public:
	enum TOPOLOGY_TYPE {
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

	IMesh(void);
	virtual ~IMesh(void);

	void destroy(void);

	template <class Vertex>
	bool addVertData(
		IRenderDevice& rd, const Vertex* vert_data, unsigned int vert_count,
		unsigned int* indices, unsigned int index_count, TOPOLOGY_TYPE primitive_type = TRIANGLE_LIST)
	{
		return addVertData(rd, vert_data, vert_count, sizeof(Vertex), indices, index_count, primitive_type);
	}

	virtual bool addVertData(
		IRenderDevice& rd, const void* vert_data, unsigned int vert_count, unsigned int vert_size,
		unsigned int* indices, unsigned int index_count, TOPOLOGY_TYPE primitive_type = TRIANGLE_LIST
	) = 0;

	virtual void addBuffer(IBuffer* buffer);
	INLINE const IBuffer* getBuffer(unsigned int index) const;
	INLINE IBuffer* getBuffer(unsigned int index);
	INLINE unsigned int getBufferCount(void) const;

	INLINE void setIndiceBuffer(IBuffer* buffer);

	virtual void setTopologyType(TOPOLOGY_TYPE topology) = 0;
	INLINE TOPOLOGY_TYPE getTopologyType(void) const;

	INLINE void setIndexCount(unsigned int count);
	INLINE unsigned int getIndexCount(void) const;

	virtual void renderNonIndexed(IRenderDevice& rd, unsigned int vert_count, unsigned int start_location = 0) = 0;
	virtual void renderInstanced(IRenderDevice& rd, unsigned int count) = 0;
	virtual void render(IRenderDevice& rd) = 0;

	virtual bool isD3D(void) const = 0;

protected:
	GleamArray<IBuffer*> _vert_data;
	IBuffer* _indices;
	TOPOLOGY_TYPE _topology;

	bool addVertDataHelper(
		IRenderDevice& rd, const void* vert_data, unsigned int vert_count, unsigned int vert_size,
		unsigned int* indices, unsigned int index_count, TOPOLOGY_TYPE primitive_type,
		IBuffer* index_buffer, IBuffer* vert_buffer
	);

private:
	unsigned int _index_count;

	GAFF_NO_COPY(IMesh);
};

NS_END
