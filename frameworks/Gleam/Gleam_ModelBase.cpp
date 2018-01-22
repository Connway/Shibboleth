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

#include "Gleam_ModelBase.h"
#include "Gleam_ILayout.h"
#include "Gleam_IMesh.h"
#include <Gaff_Assert.h>

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
	for (int32_t i = 0; i < static_cast<int32_t>(_layouts.size()); ++i) {
		_layouts[i]->release();
	}

	for (int32_t i = 0; i < static_cast<int32_t>(_meshes.size()); ++i) {
		_meshes[i]->release();
	}

	_layouts.clear();
	_meshes.clear();
}

const ILayout* ModelBase::getLayout(int32_t index) const
{
	GAFF_ASSERT(index < static_cast<int32_t>(_meshes.size()));
	return _layouts[index];
}

ILayout* ModelBase::getLayout(int32_t index)
{
	GAFF_ASSERT(index < static_cast<int32_t>(_meshes.size()));
	return _layouts[index];
}

int32_t ModelBase::getIndex(const ILayout* layout) const
{
	GAFF_ASSERT(layout);
	auto it = Gaff::Find(_layouts, layout);
	return it != _layouts.end() ? static_cast<int32_t>(it - _layouts.begin()) : -1;
}

int32_t ModelBase::addLayout(ILayout* layout)
{
	GAFF_ASSERT(layout);
	_layouts.emplace_back(layout);
	layout->addRef();
	return static_cast<int32_t>(_layouts.size() - 1);
}

const IMesh* ModelBase::getMesh(int32_t index) const
{
	GAFF_ASSERT(index < static_cast<int32_t>(_meshes.size()));
	return _meshes[index];
}

IMesh* ModelBase::getMesh(int32_t index)
{
	GAFF_ASSERT(index < static_cast<int32_t>(_meshes.size()));
	return _meshes[index];
}

int32_t ModelBase::getMeshCount(void) const
{
	return static_cast<int32_t>(_meshes.size());
}

int32_t ModelBase::getIndex(const IMesh* mesh) const
{
	GAFF_ASSERT(mesh);
	auto it = Gaff::Find(_meshes, mesh);
	return it != _meshes.end() ? static_cast<int32_t>(it - _meshes.begin()) : -1;
}

int32_t ModelBase::addMesh(IMesh* mesh)
{
	GAFF_ASSERT(mesh);
	_meshes.emplace_back(mesh);
	mesh->addRef();
	return static_cast<int32_t>(_meshes.size() - 1);
}

void ModelBase::renderInstanced(IRenderDevice& rd, int32_t index, int32_t count)
{
	GAFF_ASSERT(_layouts.size() == _meshes.size() && index < static_cast<int32_t>(_meshes.size()));
	GAFF_ASSERT(_meshes[index] && _layouts[index]);

	IMesh* mesh = _meshes[index];
	_layouts[index]->setLayout(rd, mesh);
	mesh->renderInstanced(rd, count);
}

void ModelBase::render(IRenderDevice& rd, int32_t index)
{
	GAFF_ASSERT(_layouts.size() == _meshes.size() && index < static_cast<int32_t>(_meshes.size()));
	GAFF_ASSERT(_meshes[index] && _layouts[index]);

	IMesh* mesh = _meshes[index];
	_layouts[index]->setLayout(rd, mesh);
	mesh->render(rd);
}

NS_END
