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

#pragma once

#include "Gleam_RefCounted.h"
#include "Gleam_ILayout.h"
#include "Gleam_Array.h"

NS_GLEAM

class LayoutGL : public ILayout
{
public:
	struct LayoutData
	{
		unsigned int aligned_byte_offset;
		int size;
		unsigned int type;
		bool normalized;
	};

	LayoutGL(void);
	~LayoutGL(void);

	bool init(IRenderDevice&, const LayoutDescription* layout_desc, size_t layout_desc_size, const IShader*) override;
	void destroy(void) override;

	void setLayout(IRenderDevice& rd, const IMesh* mesh) override;
	void unsetLayout(IRenderDevice& rd) override;

	RendererType getRendererType(void) const override;

	INLINE const GleamArray< GleamArray<LayoutData> >& GetLayoutDescriptors(void) const;

private:
	GleamArray< GleamArray<LayoutData> > _layout_descs;

	GLEAM_REF_COUNTED(LayoutGL);
};

NS_END