/************************************************************************************
Copyright (C) 2017 by Nicholas LaCroix

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

#include "Shibboleth_IFrameManager.h"
#include <Shibboleth_IUpdateQuery.h>
#include <Shibboleth_IManager.h>

NS_SHIBBOLETH

class CameraComponent;
class IRenderManager;

class FrameManager : public IManager, public IUpdateQuery, public IFrameManager
{
public:
	template <class T>
	T* getNextFrameDataT(size_t phase_id)
	{
		return reinterpret_cast<T*>(getNextFrameData(phase_id));
	}

	FrameManager(void);
	~FrameManager(void);

	const char* getName(void) const override;
	void allManagersCreated(void) override;

	void getUpdateEntries(Array<UpdateEntry>& entries) override;

	void setFrameDataFuncs(FrameDataAllocFunc alloc_func, FrameDataFreeFunc free_func) override;

	bool init(size_t num_frames = 16) override;
	void setNumPhases(size_t num_phases) override;

	void* getNextFrameData(size_t phase_id) override;
	void finishFrame(size_t phase_id) override;

private:
	struct CounterHack
	{
		volatile unsigned int count = 0;
	};

	Array<unsigned int> _phase_trackers;
	Array<CounterHack> _frame_trackers;

	IRenderManager* _render_mgr;
	void* _frame_data;
	FrameDataAllocFunc _frame_data_alloc;
	FrameDataFreeFunc _frame_data_free;
	size_t _frame_data_size;
	size_t _num_frames;

	void submitCommandLists(double, void* frame_data);

	GAFF_NO_COPY(FrameManager);
	GAFF_NO_MOVE(FrameManager);

	SHIB_REF_DEF(FrameManager);
	REF_DEF_REQ;
};

NS_END
