/************************************************************************************
Copyright (C) 2013 by Nicholas LaCroix

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
#include "Gaff_Defines.h"

NS_GLEAM

class IRenderTarget;
class IBuffer;
class Window;

class IRenderDevice
{
public:
	IRenderDevice(void): _prev_rt(NULLPTR) {}
	virtual ~IRenderDevice(void) {}

	virtual bool init(const Window& window, bool vsync = false, int compat1 = 28, unsigned int compat2 = 1UL) = 0;
	virtual void destroy(void) = 0;

	virtual bool isVsync(void) const = 0;
	virtual bool setVsync(bool vsync) = 0;

	virtual void setClearColor(float r, float g, float b, float a) = 0;

	virtual void beginFrame(void) = 0;
	virtual void endFrame(void) = 0;

	virtual bool resize(const Window& window) = 0;

	virtual void unbindRenderTarget(void) = 0;
	virtual void resetRenderState(void) = 0;

	virtual bool isD3D(void) const = 0;

	virtual unsigned int getViewportWidth(void) const = 0;
	virtual unsigned int getViewportHeight(void) const = 0;

	IRenderTarget* getPrevRenderTarget(void) const { return _prev_rt; }

protected:
	IRenderTarget* _prev_rt;
	friend class IRenderTarget;

	GAFF_NO_COPY(IRenderDevice);
};

NS_END
