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

#include "Gleam_Defines.h"
#include <Gaff_IRefCounted.h>

NS_GLEAM

struct LayoutDescription;
class IRenderDevice;
class IProgram;
class ILayout;
class IShader;
class IMesh;

class IModel : public Gaff::IRefCounted
{
public:
	IModel(void) {}
	~IModel(void) {}

	virtual void destroy(void) = 0;

	virtual const ILayout* getLayout(int32_t index) const = 0;
	virtual ILayout* getLayout(int32_t index) = 0;
	virtual int32_t getIndex(const ILayout* layout) const = 0;

	virtual ILayout* createLayout(IRenderDevice& rd, const LayoutDescription* layout_desc, unsigned int desc_size, const IShader* shader) = 0;
	virtual int32_t addLayout(ILayout* layout) = 0;

	virtual const IMesh* getMesh(int32_t index) const = 0;
	virtual IMesh* getMesh(int32_t index) = 0;
	virtual int32_t getMeshCount(void) const = 0;
	virtual int32_t getIndex(const IMesh* mesh) const = 0;

	virtual IMesh* createMesh(void) = 0;
	virtual int32_t addMesh(IMesh* mesh) = 0;

	virtual void renderInstanced(IRenderDevice& rd, int32_t index, int32_t count) = 0;
	virtual void render(IRenderDevice& rd, int32_t index) = 0;

	virtual RendererType getRendererType(void) const = 0;

	GAFF_NO_COPY(IModel);
};

NS_END
