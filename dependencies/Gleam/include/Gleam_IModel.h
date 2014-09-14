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

#pragma once

#include "Gleam_RefCounted.h"
#include "Gleam_Array.h"

NS_GLEAM

struct LayoutDescription;
class IRenderDevice;
class IProgram;
class ILayout;
class IShader;
class IMesh;

class IModel : public GleamRefCounted
{
public:
	IModel(void);
	~IModel(void);

	void destroy(void);

	const ILayout* getLayout(unsigned int index) const;
	ILayout* getLayout(unsigned int index);
	int getIndex(const ILayout* layout) const;

	virtual ILayout* createLayout(IRenderDevice& rd, const LayoutDescription* layout_desc, unsigned int desc_size, const IShader* shader) = 0;
	unsigned int addLayout(ILayout* layout);

	INLINE const IMesh* getMesh(unsigned int index) const;
	INLINE IMesh* getMesh(unsigned int index);
	INLINE unsigned int getMeshCount(void) const;
	int getIndex(const IMesh* mesh) const;

	virtual IMesh* createMesh(void) = 0;
	unsigned int addMesh(IMesh* mesh);

	void render(IRenderDevice& rd, unsigned int index);

private:
	GleamArray<ILayout*> _layouts;
	GleamArray<IMesh*> _meshes;

	GAFF_NO_COPY(IModel);
};

NS_END
