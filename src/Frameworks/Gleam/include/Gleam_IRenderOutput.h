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
#include <Gaff_Defines.h>

NS_GLEAM

class IRenderDevice;
class IWindow;

class IRenderOutput
{
public:
	IRenderOutput(void) {}
	virtual ~IRenderOutput(void) {}

	virtual bool init(IRenderDevice& device, const IWindow& window, int32_t output_id, int32_t mode_id, bool vsync) = 0;
	virtual bool init(IRenderDevice& device, const IWindow& window, int32_t output_id, bool vsync) = 0;

	virtual RendererType getRendererType(void) const = 0;

	GAFF_NO_COPY(IRenderOutput);
};

NS_END