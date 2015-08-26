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

#include "Shibboleth_OcclusionManager.h"
#include <Shibboleth_CameraComponent.h>
#include <Shibboleth_IManager.h>
#include <Gleam_Transform_CPU.h>
#include <Gleam_ICommandList.h>
#include <Gaff_SmartPtr.h>

NS_SHIBBOLETH

class CameraComponent;
class RenderManager;

struct FrameData
{
	using CommandListPtr = Gaff::SmartPtr<Gleam::ICommandList, ProxyAllocator>;
	using CommandListData = Array< Array< Array<CommandListPtr> > >; // [Device][RenderMode][CommandList]
	using CommandListMap = Map<CameraComponent*, CommandListData>;

	struct ObjectData
	{
		Array<Gleam::TransformCPU> transforms[OcclusionManager::OT_SIZE];
		Gleam::TransformCPU camera_transform;
		OcclusionManager::QueryData objects;
		CameraComponent* camera;

		volatile unsigned int next_index[OcclusionManager::OT_SIZE];
	};

	Array<ObjectData> object_data;
	Map<unsigned int, CommandListMap> command_lists; // Key is thread ID

	// animation transforms
};

class FrameManager : public IManager, public IUpdateQuery
{
public:
	using FrameDataAllocFunc = void* (*)(size_t, size_t&);
	using FrameDataFreeFunc = void (*)(void*, size_t);

	template <class T>
	INLINE T* getNextFrameDataT(size_t phase_id)
	{
		return reinterpret_cast<T*>(getNextFrameData(phase_id));
	}

	FrameManager(void);
	~FrameManager(void);

	const char* getName(void) const override;
	void allManagersCreated(void) override;

	void* rawRequestInterface(unsigned int class_id) const;
	void getUpdateEntries(Array<UpdateEntry>& entries);

	void setFrameDataFuncs(FrameDataAllocFunc alloc_func, FrameDataFreeFunc free_func);

	bool init(size_t num_frames = 16);
	void setNumPhases(size_t num_phases);

	void* getNextFrameData(size_t phase_id);
	void finishFrame(size_t phase_id);

private:
	class CameraPredicate
	{
	public:
		bool operator()(const Gaff::Pair<CameraComponent*, FrameData::CommandListData>& lhs, const CameraComponent* rhs) const
		{
			return lhs.first->getRenderOrder() < rhs->getRenderOrder();
		}
	};

	struct CounterHack
	{
		volatile unsigned int count = 0;
	};

	// Sort Map by camera render order.
	Map<CameraComponent*, FrameData::CommandListData, CameraPredicate> _cl_cache;

	Array<unsigned int> _phase_trackers;
	Array<CounterHack> _frame_trackers;

	RenderManager* _render_mgr;
	void* _frame_data;
	FrameDataAllocFunc _frame_data_alloc;
	FrameDataFreeFunc _frame_data_free;
	size_t _frame_data_size;
	size_t _num_frames;

	void submitCommandLists(double, void* frame_data);

	GAFF_NO_COPY(FrameManager);
	GAFF_NO_MOVE(FrameManager);

	SHIB_REF_DEF(FrameManager);
};

NS_END
