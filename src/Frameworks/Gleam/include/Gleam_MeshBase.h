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
#include "Gleam_Vector.h"
#include "Gleam_IMesh.h"

NS_GLEAM

class MeshBase : public IMesh
{
public:
	MeshBase(void);
	~MeshBase(void);

	void destroy(void) override;

	void addBuffer(IBuffer* buffer) override;
	const IBuffer* getBuffer(int32_t index) const override;
	IBuffer* getBuffer(int32_t index) override;
	int32_t getBufferCount(void) const override;

	void setIndiceBuffer(IBuffer* buffer) override;
	const IBuffer* getIndiceBuffer(void) const override;
	IBuffer* getIndiceBuffer(void) override;

	TopologyType getTopologyType(void) const override;

	void setIndexCount(int32_t count) override;
	int32_t getIndexCount(void) const override;

protected:
	Vector<IBuffer*> _vert_data;
	IBuffer* _indices;
	TopologyType _topology;

	bool addVertDataHelper(
		IRenderDevice& rd, const void* vert_data, int32_t vert_count, int32_t vert_size,
		int32_t* indices, int32_t index_count, TopologyType primitive_type,
		IBuffer* index_buffer, IBuffer* vert_buffer
	);

private:
	int32_t _index_count;

	GLEAM_REF_COUNTED(MeshBase);
};

NS_END