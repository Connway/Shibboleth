/************************************************************************************
Copyright (C) 2013 by Nicholas LaCroix

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

void MeshGL::setTopologyType(TOPOLOGY_TYPE topology)
{
	_gl_topology = _topology_map[topology];
	_topology = topology;
}

void MeshGL::render(const IRenderDevice&)
{
	assert(_vert_data.size() && _indices && !_indices->isD3D());
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ((BufferGL*)_indices)->getBuffer());
	glDrawElements(_gl_topology, getIndexCount(), GL_UNSIGNED_INT, 0);
}

bool MeshGL::isD3D(void) const
{
	return false;
}

NS_END
