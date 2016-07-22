/************************************************************************************
Copyright (C) 2016 by Nicholas LaCroix

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

#include "Gleam_Model_OpenGL.h"
#include "Gleam_Layout_OpenGL.h"
#include "Gleam_Mesh_OpenGL.h"

NS_GLEAM

ModelGL::ModelGL(void)
{
}

ModelGL::~ModelGL(void)
{
}

ILayout* ModelGL::createLayout(IRenderDevice& rd, const LayoutDescription* layout_desc, unsigned int desc_size, const IShader* shader)
{
	ILayout* layout = GleamAllocateT(LayoutGL);

	if (!layout) {
		return nullptr;
	}

	layout->addRef();

	if (!layout->init(rd, layout_desc, desc_size, shader)) {
		layout->release();
		return nullptr;
	}

	addLayout(layout);
	layout->release();

	return layout;
}

IMesh* ModelGL::createMesh(void)
{
	IMesh* mesh = GleamAllocateT(MeshGL);

	if (!mesh) {
		return nullptr;
	}

	addMesh(mesh);

	return mesh;
}

RendererType ModelGL::getRendererType(void) const
{
	return RENDERER_OPENGL;
}

NS_END
