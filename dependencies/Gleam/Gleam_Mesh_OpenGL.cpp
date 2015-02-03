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

#include "Gleam_Mesh_OpenGL.h"
#include "Gleam_IRenderDevice_OpenGL.h"
#include "Gleam_IRenderDevice.h"
#include "Gleam_Buffer_OpenGL.h"
#include <GL/glew.h>

NS_GLEAM

static GLenum _topology_map[IMesh::TOPOLOGY_SIZE] = {
	GL_POINTS,
	GL_LINES,
	GL_LINE_STRIP,
	GL_TRIANGLES,
	GL_TRIANGLE_STRIP,

	GL_LINES_ADJACENCY,
	GL_LINE_STRIP_ADJACENCY,
	GL_TRIANGLES_ADJACENCY,
	GL_TRIANGLE_STRIP_ADJACENCY
};

MeshGL::MeshGL(void):
	_gl_topology(0)
{
}

MeshGL::~MeshGL(void)
{
	destroy();
}

bool MeshGL::addVertData(
	IRenderDevice& rd, const void* vert_data, unsigned int vert_count, unsigned int vert_size,
	unsigned int* indices, unsigned int index_count, TOPOLOGY_TYPE primitive_type
)
{
	IBuffer* index_buffer = GleamAllocateT(BufferGL);
	IBuffer* vert_buffer = GleamAllocateT(BufferGL);

	if (!index_buffer || !vert_buffer) {
		if (index_buffer) {
			GetAllocator()->freeT(index_buffer);
		}

		if (vert_buffer) {
			GetAllocator()->freeT(vert_buffer);
		}

		return false;
	}

	bool ret = addVertDataHelper(
		rd, vert_data, vert_count, vert_size, indices, index_count,
		primitive_type, index_buffer, vert_buffer
	);

	if (!ret) {
		GetAllocator()->freeT(index_buffer);
		GetAllocator()->freeT(vert_buffer);
	} else {
		glFinish();
	}

	return ret;
}

void MeshGL::setTopologyType(TOPOLOGY_TYPE topology)
{
	_gl_topology = _topology_map[topology];
	_topology = topology;
}

void MeshGL::renderNonIndexed(IRenderDevice& rd, unsigned int vert_count, unsigned int start_location)
{
	assert(_vert_data.size());
	IRenderDeviceGL& rdgl = (IRenderDeviceGL&)*(((const char*)&rd) + sizeof(IRenderDevice));
	rdgl.renderMeshNonIndexed(_gl_topology, vert_count, start_location);

}

void MeshGL::renderInstanced(IRenderDevice& rd, unsigned int count)
{
	assert(_vert_data.size() && _indices && !_indices->isD3D());
	IRenderDeviceGL& rdgl = (IRenderDeviceGL&)*(((const char*)&rd) + sizeof(IRenderDevice));
	rdgl.renderMeshInstanced(this, count);
}

void MeshGL::render(IRenderDevice& rd)
{
	assert(_vert_data.size() && _indices && !_indices->isD3D() && !rd.isD3D());
	IRenderDeviceGL& rdgl = (IRenderDeviceGL&)*(((const char*)&rd) + sizeof(IRenderDevice));
	rdgl.renderMesh(this);
}

bool MeshGL::isD3D(void) const
{
	return false;
}

unsigned int MeshGL::getGLTopology(void) const
{
	return _gl_topology;
}

NS_END
