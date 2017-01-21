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

#if defined(_WIN32) || defined(_WIN64)

#include "Gleam_Model_Direct3D.h"
#include "Gleam_Layout_Direct3D.h"
#include "Gleam_Mesh_Direct3D.h"

NS_GLEAM

ModelD3D::ModelD3D(void)
{
}

ModelD3D::~ModelD3D(void)
{
}

ILayout* ModelD3D::createLayout(IRenderDevice& rd, const LayoutDescription* layout_desc, unsigned int desc_size, const IShader* shader)
{
	ILayout* layout = GleamAllocateT(LayoutD3D);

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

IMesh* ModelD3D::createMesh(void)
{
	IMesh* mesh = GleamAllocateT(MeshD3D);

	if (!mesh) {
		return nullptr;
	}

	addMesh(mesh);

	return mesh;
}

RendererType ModelD3D::getRendererType(void) const
{
	return RENDERER_DIRECT3D;
}

NS_END

#endif
