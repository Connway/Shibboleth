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

#include <Shibboleth_IManager.h>

NS_SHIBBOLETH

class FrameManager : public IManager
{
public:
	struct FrameData
	{
	};

	using FrameDataAllocFunc = void* (*)(size_t, size_t&);
	using FrameDataFreeFunc = void (*)(void*, size_t);

	FrameManager(void);
	~FrameManager(void);

	void setFrameDataFuncs(FrameDataAllocFunc alloc_func, FrameDataFreeFunc free_func);

	bool init(size_t num_frames);
	void setNumPhases(size_t num_phases);

private:
	Array<volatile unsigned int> _phase_trackers;
	Array<volatile unsigned int> _frame_trackers;

	void* _frame_data;
	FrameDataAllocFunc _frame_data_alloc;
	FrameDataFreeFunc _frame_data_free;
	size_t _frame_data_size;

	volatile unsigned int _starting_frame_check;

	static void* DefaultFrameDataAlloc(size_t num_frames, size_t& frame_data_size);
	static void DefaultFrameDataFree(void* frame_data, size_t num_frames);
};

NS_END
