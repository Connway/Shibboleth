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

#include "Shibboleth_FrameManager.h"
#include "Shibboleth_IRenderManager.h"
#include <Shibboleth_Utilities.h>
#include <Shibboleth_IApp.h>
#include <Gleam_IRenderDevice.h>
#include <Gleam_ICommandList.h>
#include <Gleam_IProgram.h>
#include <Gleam_IModel.h>
#include <Gaff_Atomic.h>

NS_SHIBBOLETH

// Default frame data functions
static void* DefaultFrameDataAlloc(size_t num_frames, size_t& frame_data_size)
{
	frame_data_size = sizeof(FrameData);
	return SHIB_ALLOC_ARRAYT(FrameData, *GetAllocator(), num_frames);
}

static void DefaultFrameDataFree(void* frame_data, size_t num_frames)
{
	FrameData* fd = reinterpret_cast<FrameData*>(frame_data);
	SHIB_FREE_ARRAYT(fd, num_frames, *GetAllocator());
}


REF_IMPL_REQ(FrameManager);
SHIB_REF_IMPL(FrameManager)
.addBaseClassInterfaceOnly<FrameManager>()
.ADD_BASE_CLASS_INTERFACE_ONLY(IFrameManager)
.ADD_BASE_CLASS_INTERFACE_ONLY(IUpdateQuery)
;

FrameManager::FrameManager(void):
	_render_mgr(nullptr), _frame_data(nullptr),
	_frame_data_alloc(&DefaultFrameDataAlloc),
	_frame_data_free(&DefaultFrameDataFree), _frame_data_size(0),
	_num_frames(0)
{
}

FrameManager::~FrameManager(void)
{
	if (_frame_data) {
		_frame_data_free(_frame_data, _num_frames);
	}
}

const char* FrameManager::getName(void) const
{
	return GetFriendlyName();
}

void FrameManager::allManagersCreated(void)
{
	_render_mgr = &GetApp().getManagerT<IRenderManager>();
}

void FrameManager::getUpdateEntries(Array<UpdateEntry>& entries)
{
	entries.emplacePush(U8String("Frame Manager: Submit Command Lists"), Gaff::Bind(this, &FrameManager::submitCommandLists));
}

void FrameManager::setFrameDataFuncs(FrameDataAllocFunc alloc_func, FrameDataFreeFunc free_func)
{
	_frame_data_alloc = alloc_func;
	_frame_data_free = free_func;
}

bool FrameManager::init(size_t num_frames)
{
	GAFF_ASSERT(_frame_data_alloc && _frame_data_free);
	_frame_data = _frame_data_alloc(num_frames, _frame_data_size);
	_frame_trackers.resize(num_frames);
	_num_frames = num_frames;

	return _frame_data != nullptr;
}

void FrameManager::setNumPhases(size_t num_phases)
{
	_phase_trackers.resize(num_phases, 0);
}

void* FrameManager::getNextFrameData(size_t phase_id)
{
	GAFF_ASSERT(phase_id < _phase_trackers.size());

	unsigned int frame_id = _phase_trackers[phase_id];
	GAFF_ASSERT(frame_id < _frame_trackers.size());

	// If the frame tracker count is not the same as our phase ID,
	// then we have used up all the frames. Wait until a frame opens up.
	if (_frame_trackers[frame_id].count != phase_id) {
		return nullptr;
	}

	return reinterpret_cast<char*>(_frame_data) + (_frame_data_size * frame_id);
}

void FrameManager::finishFrame(size_t phase_id)
{
	GAFF_ASSERT(phase_id < _phase_trackers.size());

	unsigned int& frame_id = _phase_trackers[phase_id];
	GAFF_ASSERT(frame_id < _frame_trackers.size());

	unsigned int new_val = AtomicIncrement(&_frame_trackers[frame_id].count);

	// If every phase has processed this frame, then open it up for use.
	if (new_val == _phase_trackers.size()) {
		AtomicExchange(&_frame_trackers[frame_id].count, 0);
	}

	// Set the frame ID for the phase to the next frame.
	frame_id = (frame_id + 1) % _num_frames;
}

void FrameManager::submitCommandLists(double, void* frame_data)
{
	FrameData* fd = reinterpret_cast<FrameData*>(frame_data);
	Gleam::IRenderDevice& rd = _render_mgr->getRenderDevice();

	for (auto it_cam = fd->camera_object_data.begin(); it_cam != fd->camera_object_data.end(); ++it_cam) {
		for (size_t i = 0; i < it_cam->second.command_lists.size(); ++i) {
			rd.setCurrentDevice(static_cast<unsigned int>(i));
			const auto& cmds = it_cam->second.command_lists[i];

			for (auto it_cmd = cmds.begin(); it_cmd != cmds.end(); ++it_cmd) {
				rd.executeCommandList(it_cmd->get());
			}
		}
	}
}

NS_END
