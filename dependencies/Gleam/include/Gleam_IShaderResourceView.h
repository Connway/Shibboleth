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

#include "Gleam_Defines.h"
#include <Gaff_RefCounted.h>

NS_GLEAM

class IRenderDevice;
class ITexture;
class IBuffer;

class IShaderResourceView : public Gaff::IRefCounted
{
public:
	enum RES_VIEW_TYPE { VIEW_TEXTURE = 0, VIEW_BUFFER };

	IShaderResourceView(void) {}
	virtual ~IShaderResourceView(void) {}

	virtual bool init(IRenderDevice& rd, const ITexture* texture) = 0;
	virtual bool init(IRenderDevice& rd, const IBuffer* buffer) = 0;
	virtual void destroy(void) = 0;

	INLINE RES_VIEW_TYPE getViewType(void) const { return _view_type; }

	virtual bool isD3D(void) const = 0;

protected:
	RES_VIEW_TYPE _view_type;

	GAFF_NO_COPY(IShaderResourceView);
};

NS_END
