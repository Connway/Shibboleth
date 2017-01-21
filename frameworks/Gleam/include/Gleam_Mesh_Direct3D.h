/************************************************************************************
Copyright (C) 2017 by Nicholas LaCroix

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

#include "Gleam_IncludeD3D11.h"
#include "Gleam_MeshBase.h"

NS_GLEAM

class MeshD3D : public MeshBase
{
public:
	MeshD3D(void);
	~MeshD3D(void);

	bool addVertData(
		IRenderDevice& rd, const void* vert_data, unsigned int vert_count, unsigned int vert_size,
		unsigned int* indices, unsigned int index_count, TOPOLOGY_TYPE primitive_type = TRIANGLE_LIST
	);

	void addBuffer(IBuffer* buffer);

	void setTopologyType(TOPOLOGY_TYPE topology);
	void renderNonIndexed(IRenderDevice& rd, unsigned int vert_count, unsigned int start_location = 0);
	void renderInstanced(IRenderDevice& rd, unsigned int count);
	void render(IRenderDevice& rd);

	RendererType getRendererType(void) const;

private:
	D3D11_PRIMITIVE_TOPOLOGY _d3d_topology;

	GleamArray<ID3D11Buffer*> _buffers;
	GleamArray<unsigned int> _strides;
	GleamArray<unsigned int> _offsets;

	void cacheBuffers(void);
};

NS_END
