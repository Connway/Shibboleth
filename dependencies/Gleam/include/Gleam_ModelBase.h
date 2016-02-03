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

#pragma once

#include "Gleam_IModel.h"
#include "Gleam_RefCounted.h"
#include "Gleam_Array.h"

NS_GLEAM

class ModelBase : public IModel
{
public:
	ModelBase(void);
	~ModelBase(void);

	void destroy(void);

	const ILayout* getLayout(size_t index) const;
	ILayout* getLayout(size_t index);
	size_t getIndex(const ILayout* layout) const;

	size_t addLayout(ILayout* layout);

	const IMesh* getMesh(size_t index) const;
	IMesh* getMesh(size_t index);
	size_t getMeshCount(void) const;
	size_t getIndex(const IMesh* mesh) const;

	size_t addMesh(IMesh* mesh);

	void renderInstanced(IRenderDevice& rd, size_t index, unsigned int count);
	void render(IRenderDevice& rd, size_t index);

private:
	GleamArray<ILayout*> _layouts;
	GleamArray<IMesh*> _meshes;

	GLEAM_REF_COUNTED(ModelBase);
};

NS_END
