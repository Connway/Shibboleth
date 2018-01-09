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

#include <Shibboleth_ResourceDefines.h>
#include <Shibboleth_CameraComponent.h>
#include <Shibboleth_Map.h>
#include <Gleam_Transform_CPU.h>
//#include <Gleam_ICommandList.h>
#include <Gaff_SmartPtr.h>

NS_GLEAM
	class ICommandList;
NS_END

NS_SHIBBOLETH

class CameraComponent;

struct ObjectData
{
	//struct Test
	//{
	//	Array<ProgramBuffersPtr> program_buffers; // [Mesh]
	//	Array<ProgramPtr> programs; // [Mesh]
	//	ModelPtr model;
	//	Gleam::TransformCPU transform;
	//};

	using CommandListPtr = Gaff::SmartPtr<Gleam::ICommandList, ProxyAllocator>;

	bool active;
	volatile unsigned int curr_device;

	Array< Array<RasterStatePtr> > raster_states; // [Mesh][Device]
	Array< Array<ProgramBuffersPtr> > program_buffers; // [Mesh][Device]
	Array< Array<ProgramPtr> > programs; // [Mesh][Device]
	Array<RenderPasses> render_pass; // [Mesh]
	Array< Array<ModelPtr> > models; // [Object][Device]
	Array<Gleam::TransformCPU> transforms; // [Object]

										   //Array<Gleam::TransformCPU> transforms[OcclusionManager::OT_SIZE];
	Gleam::Matrix4x4CPU projection_matrix;
	Gleam::TransformCPU eye_transform;
	Gleam::TransformCPU inv_eye_transform;
	float clear_color[4];
	int clear_mode;

	Array< Array<CommandListPtr> > command_lists; // [Device][CmdList]
	Array<Gaff::SpinLock> cmd_lists_locks; // [Device]
};

class FramePredicate
{
public:
	bool operator()(const Gaff::Pair<CameraComponent*, ObjectData>& lhs, const CameraComponent* rhs) const
	{
		return lhs.first->getRenderOrder() < rhs->getRenderOrder();
	}
};

struct FrameData
{
	Map<CameraComponent*, ObjectData, FramePredicate> camera_object_data;
	//Map<LightComponent*, ObjectData> shadow_object_data;
};

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
	SHIB_INTERFACE_NAME("Frame Manager")
};

NS_END
