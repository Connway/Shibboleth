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

#include "Gleam_Model.h"
#include "Gleam_Layout.h"
#include "Gleam_Mesh.h"

NS_GLEAM

Model::Model(void)
{
}

Model::~Model(void)
{
}

ILayout* Model::createLayout(IRenderDevice& rd, const LayoutDescription* layout_desc, unsigned int desc_size, const IShader* shader)
{
	ILayout* layout = GleamAllocateT(Layout);

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

IMesh* Model::createMesh(void)
{
	IMesh* mesh = GleamAllocateT(Mesh);

	if (!mesh) {
		return nullptr;
	}

	addMesh(mesh);

	return mesh;
}

NS_END
