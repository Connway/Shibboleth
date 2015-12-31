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

#include <Shibboleth_IRenderPipeline.h>
#include <Shibboleth_RenderManager.h>
#include <Shibboleth_JobPool.h>
#include <Shibboleth_Array.h>
#include <Gleam_IDepthStencilState.h>
#include <Gleam_IBlendState.h>
#include <Gaff_SmartPtr.h>

NS_SHIBBOLETH

class RenderManager;
struct ObjectData;
struct FrameData;

class InGameRenderPipeline : public IRenderPipeline
{
public:
	InGameRenderPipeline(void);
	~InGameRenderPipeline(void);

	bool init(void);

	const char* getName(void) const override;
	void run(double dt, void* frame_data) override;

private:
	using IDepthStencilStatePtr = Gaff::SmartPtr<Gleam::IDepthStencilState, ProxyAllocator>;
	using IBlendStatePtr = Gaff::SmartPtr<Gleam::IBlendState, ProxyAllocator>;
	using GenerateJobData = Gaff::Pair<InGameRenderPipeline*, FrameData*>;

	static void GenerateCommandLists(void* job_data);
	static void GenerateCameraCommandLists(Array<RenderManager::RenderDevicePtr>& rds, GenerateJobData* jd);
	static void GenerateLightCommandLists(Array<RenderManager::RenderDevicePtr>& rds, GenerateJobData* jd);

	static void SortIntoRenderPasses(ObjectData& od, unsigned int device);
	static void RunCommands(Gleam::IRenderDevice* rd, GenerateJobData* jd, unsigned int device);

	// Array size is number of devices
	Array<IDepthStencilStatePtr> _ds_states[RP_COUNT];
	Array<IBlendStatePtr> _blend_states[2]; // On and off

	Array<Gaff::JobData> _job_cache;
	Gaff::Counter* _counter;

	RenderManager& _render_mgr;

	GAFF_NO_COPY(InGameRenderPipeline);
	GAFF_NO_MOVE(InGameRenderPipeline);
};

NS_END
