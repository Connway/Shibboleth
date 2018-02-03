/************************************************************************************
Copyright (C) 2018 by Nicholas LaCroix

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

#include "Gleam_RefCounted.h"
#include "Gleam_Vector.h"
#include "Gleam_IModel.h"

NS_GLEAM

class ModelBase : public IModel
{
public:
	ModelBase(void);
	~ModelBase(void);

	void destroy(void) override;

	const ILayout* getLayout(int32_t index) const override;
	ILayout* getLayout(int32_t index) override;
	int32_t getIndex(const ILayout* layout) const override;

	int32_t addLayout(ILayout* layout) override;

	const IMesh* getMesh(int32_t index) const override;
	IMesh* getMesh(int32_t index) override;
	int32_t getMeshCount(void) const override;
	int32_t getIndex(const IMesh* mesh) const override;

	int32_t addMesh(IMesh* mesh) override;

	void renderInstanced(IRenderDevice& rd, int32_t index, int32_t count) override;
	void render(IRenderDevice& rd, int32_t index) override;

private:
	Vector<ILayout*> _layouts;
	Vector<IMesh*> _meshes;

	GLEAM_REF_COUNTED(ModelBase);
};

NS_END
