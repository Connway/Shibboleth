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

#include "Gleam_ModelBase.h"
#include "Gleam_ILayout.h"
#include "Gleam_IMesh.h"
#include <Gaff_IncludeAssert.h>

NS_GLEAM

ModelBase::ModelBase(void)
{
}

ModelBase::~ModelBase(void)
{
	destroy();
}

void ModelBase::destroy(void)
{
	for (unsigned int i = 0; i < _layouts.size(); ++i) {
		_layouts[i]->release();
	}

	for (unsigned int i = 0; i < _meshes.size(); ++i) {
		_meshes[i]->release();
	}

	_layouts.clear();
	_meshes.clear();
}

const ILayout* ModelBase::getLayout(size_t index) const
{
	assert(index < _meshes.size());
	return _layouts[index];
}

ILayout* ModelBase::getLayout(size_t index)
{
	assert(index < _meshes.size());
	return _layouts[index];
}

size_t ModelBase::getIndex(const ILayout* layout) const
{
	assert(layout);
	return _layouts.linearSearch(0, _layouts.size(), layout);
}

size_t ModelBase::addLayout(ILayout* layout)
{
	assert(layout);
	_layouts.push(layout);
	layout->addRef();
	return _layouts.size() - 1;
}

const IMesh* ModelBase::getMesh(size_t index) const
{
	assert(index < _meshes.size());
	return _meshes[index];
}

IMesh* ModelBase::getMesh(size_t index)
{
	assert(index < _meshes.size());
	return _meshes[index];
}

size_t ModelBase::getMeshCount(void) const
{
	return _meshes.size();
}

size_t ModelBase::getIndex(const IMesh* mesh) const
{
	assert(mesh);
	return _meshes.linearSearch(0, _meshes.size(), mesh);
}

size_t ModelBase::addMesh(IMesh* mesh)
{
	assert(mesh);
	_meshes.push(mesh);
	mesh->addRef();
	return _meshes.size() - 1;
}

void ModelBase::render(IRenderDevice& rd, size_t index)
{
	assert(_layouts.size() == _meshes.size() && index < _meshes.size());
	assert(_meshes[index] && _layouts[index]);

	IMesh* mesh = _meshes[index];
	_layouts[index]->setLayout(rd, mesh);
	mesh->render(rd);
}

NS_END
