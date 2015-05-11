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

#include "Shibboleth_FrameManager.h"

NS_SHIBBOLETH

FrameManager::FrameManager(void):
	_frame_data(nullptr), _frame_data_alloc(&FrameManager::DefaultFrameDataAlloc),
	_frame_data_free(&FrameManager::DefaultFrameDataFree), _frame_data_size(0),
	_starting_frame_check(0)
{
}

FrameManager::~FrameManager(void)
{
}

void FrameManager::setFrameDataProvider(FrameDataAllocFunc alloc_func, FrameDataFreeFunc free_func)
{
	_frame_data_alloc = alloc_func;
	_frame_data_free = free_func;
}

bool FrameManager::init(size_t num_frames)
{
	assert(_frame_data_alloc && _frame_data_free);
	_frame_data = _frame_data_alloc(num_frames, _frame_data_size);
	_frame_trackers.resize(num_frames, 0);
	return _frame_data != nullptr;
}

void FrameManager::setNumPhases(size_t num_phases)
{
	_phase_trackers.resize(num_phases, 0);
}

void* FrameManager::DefaultFrameDataAlloc(size_t num_frames, size_t& frame_data_size)
{
	frame_data_size = sizeof(FrameData);
	return GetAllocator()->template allocArrayT<FrameData>(num_frames);
}

void FrameManager::DefaultFrameDataFree(void* frame_data, size_t num_frames)
{
	FrameData* fd = reinterpret_cast<FrameData*>(frame_data);
	GetAllocator->freeArrayT(fd, num_frames);
}

NS_END
