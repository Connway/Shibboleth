/************************************************************************************
Copyright (C) 2018 by Nicholas LaCroix

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
#include "Gleam_IMesh.h"
#include "Gleam_Array.h"

NS_GLEAM

class MeshBase : public IMesh
{
public:
	MeshBase(void);
	~MeshBase(void);

	void destroy(void);

	void addBuffer(IBuffer* buffer);
	const IBuffer* getBuffer(unsigned int index) const;
	IBuffer* getBuffer(unsigned int index);
	size_t getBufferCount(void) const;

	void setIndiceBuffer(IBuffer* buffer);
	const IBuffer* getIndiceBuffer(void) const;
	IBuffer* getIndiceBuffer(void);

	TOPOLOGY_TYPE getTopologyType(void) const;

	void setIndexCount(unsigned int count);
	unsigned int getIndexCount(void) const;

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

	GLEAM_REF_COUNTED(MeshBase);
};

NS_END
