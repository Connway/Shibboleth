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

#include "Gleam_IRenderDevice.h"
#include "Gaff_IncludeWindows.h"

NS_GLEAM

class RenderDeviceGL : public IRenderDevice
{
public:
	RenderDeviceGL(void);
	~RenderDeviceGL(void);

	static bool CheckRequiredExtensions(void);

	bool init(const Window& window, bool vsync = false, int compat1 = 28, unsigned int compat2 = 1UL);
	void destroy(void);

	INLINE HGLRC getDeviceContext(void) const;

	INLINE bool isVsync(void) const;
	bool setVsync(bool vsync);

	INLINE void setClearColor(float r, float g, float b, float a);

	INLINE void beginFrame(void);
	INLINE void endFrame(void);

	bool resize(const Window& window);

	INLINE void unbindRenderTarget(void);
	void resetRenderState(void);

	INLINE bool isD3D(void) const;

	INLINE unsigned int getViewportWidth(void) const;
	INLINE unsigned int getViewportHeight(void) const;

private:
	unsigned int _width, _height;
	bool _vsync;

	HGLRC _context;
	HWND _hwnd;
	HDC _hdc;
};

NS_END
