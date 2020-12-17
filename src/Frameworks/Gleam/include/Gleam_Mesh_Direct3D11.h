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

#include "Gleam_IncludeD3D11.h"
#include "Gleam_MeshBase.h"

NS_GLEAM

class MeshD3D11 : public MeshBase
{
public:
	MeshD3D11(void);
	~MeshD3D11(void);

	void clear(void) override;

	void addBuffer(IBuffer* buffer, uint32_t offset = 0) override;

	void setIndiceBuffer(IBuffer* buffer) override;

	void setTopologyType(TopologyType topology) override;
	void renderNonIndexed(IRenderDevice& rd, int32_t vert_count, int32_t vert_offset = 0) override;
	void renderInstanced(IRenderDevice& rd, int32_t instance_count, int32_t index_offset = 0, int32_t vert_offset = 0, int32_t instance_offset = 0) override;
	void render(IRenderDevice& rd, int32_t index_offset = 0, int32_t vert_offset = 0) override;

	RendererType getRendererType(void) const;

private:
	D3D11_PRIMITIVE_TOPOLOGY _d3d_topology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	DXGI_FORMAT _indice_format = DXGI_FORMAT_R16_UINT;

	Vector<ID3D11Buffer*> _buffers;
	Vector<UINT> _strides;

	void cacheBuffers(void);
};

NS_END
