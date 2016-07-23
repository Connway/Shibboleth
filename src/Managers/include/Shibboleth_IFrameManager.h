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

#include <Shibboleth_ReflectionDefinitions.h>

NS_SHIBBOLETH

class IFrameManager
{
public:
	using FrameDataAllocFunc = void* (*)(size_t, size_t&);
	using FrameDataFreeFunc = void(*)(void*, size_t);

	IFrameManager(void) {}
	virtual ~IFrameManager(void) {}

	virtual void setFrameDataFuncs(FrameDataAllocFunc alloc_func, FrameDataFreeFunc free_func) = 0;

	virtual bool init(size_t num_frames = 16) = 0;
	virtual void setNumPhases(size_t num_phases) = 0;

	virtual void* getNextFrameData(size_t phase_id) = 0;
	virtual void finishFrame(size_t phase_id) = 0;

	SHIB_INTERFACE_REFLECTION(IFrameManager)
	SHIB_INTERFACE_MANAGER("Frame Manager")
};

NS_END
