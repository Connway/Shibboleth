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

#include <Shibboleth_IRenderPipeline.h>
#include <Shibboleth_IRenderManager.h>
#include <Shibboleth_JobPool.h>
#include <Shibboleth_Array.h>
#include <Gleam_IDepthStencilState.h>
#include <Gleam_IRenderTarget.h>
#include <Gleam_IBlendState.h>

NS_SHIBBOLETH

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
	using IDepthStencilStatePtr = Gaff::RefPtr<Gleam::IDepthStencilState>;
	using IBlendStatePtr = Gaff::RefPtr<Gleam::IBlendState>;
	using GenerateJobData = Gaff::Pair<InGameRenderPipeline*, FrameData*>;

	static void GenerateCommandLists(void* job_data);
	static void GenerateCameraCommandLists(Array<RenderDevicePtr>& rds, GenerateJobData* jd);
	static void GenerateLightCommandLists(Array<RenderDevicePtr>& rds, GenerateJobData* jd);

	static void SortIntoRenderPasses(ObjectData& od, unsigned int device);
	static void RunCommands(Gleam::IRenderDevice* rd, GenerateJobData* jd, unsigned int device, const ObjectData& od);
	static void ClearCamera(Gleam::IRenderDevice* rd, ObjectData& od, Gleam::IRenderTargetPtr& rt);
	static unsigned int GenerateFirstInstanceHash(ObjectData& od, size_t mesh_index, unsigned int device);
	static unsigned int GenerateSecondInstanceHash(ObjectData& od, size_t mesh_index, unsigned int device, size_t submesh_index, unsigned int hash_init);
	static Gleam::IBuffer* CreateInstanceBuffer(IRenderManager& render_mgr, unsigned int device, unsigned int num_elements);
	static Gleam::IShaderResourceView* CreateInstanceResourceView(IRenderManager& render_mgr, Gleam::IBuffer* buffer);

	// Array size is number of devices
	Array<IDepthStencilStatePtr> _ds_states[RP_COUNT];
	Array<IBlendStatePtr> _blend_states[2]; // On and off

	Array<Gaff::JobData> _job_cache;
	Gaff::Counter* _counter;

	IRenderManager& _render_mgr;

	GAFF_NO_COPY(InGameRenderPipeline);
	GAFF_NO_MOVE(InGameRenderPipeline);
};

NS_END
