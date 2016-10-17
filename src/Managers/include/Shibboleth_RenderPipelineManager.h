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

#include "Shibboleth_IRenderPipelineManager.h"
#include "Shibboleth_IRenderManager.h"
#include <Shibboleth_ResourceDefines.h>
#include <Shibboleth_DynamicLoader.h>
#include <Shibboleth_IUpdateQuery.h>
#include <Shibboleth_IManager.h>
#include <Shibboleth_HashMap.h>

NS_SHIBBOLETH

class IRenderPipeline;
class IApp;

class RenderPipelineManager : public IManager, public IUpdateQuery, public IRenderPipelineManager
{
public:
	RenderPipelineManager(void);
	~RenderPipelineManager(void);

	const char* getName(void) const override;
	void allManagersCreated(void) override;

	void getUpdateEntries(Array<UpdateEntry>& entries) override;

	bool init(const char* initial_pipeline) override;

	void setOutputCamera(ICameraComponent* camera) override;
	ICameraComponent* getOutputCamera(unsigned int monitor) const override;
	void refreshMonitors(void) override;

	size_t getActivePipeline(void) const override;
	void setActivePipeline(size_t pipeline) override;

	size_t getPipelineIndex(const char* name) const override;

private:
	struct CameraData
	{
		ICameraComponent* camera;
		unsigned int device;
		unsigned int output;
	};

	using InitFunc = bool (*)(IApp&);
	using CreateRenderPipelineFunc = IRenderPipeline* (*)(size_t);
	using DestroyRenderPipelineFunc = void (*)(IRenderPipeline*);
	using GetNumRenderPipelinesFunc = size_t (*)(void);

	Array<CameraData> _output_cameras; // Array size == num of monitors. Element is camera that is being outputted to monitor.
	Array<IRenderPipeline*> _pipelines;
	HashMap<HashString32, DestroyRenderPipelineFunc> _pipeline_map;
	size_t _active_pipeline;

	ResourceWrapper<ProgramBuffersData> _camera_to_screen_program_buffers;
	ResourceWrapper<SamplerStateData> _camera_to_screen_sampler;
	ResourceWrapper<ProgramData> _camera_to_screen_shader;

	IRenderManager::WindowRenderTargets _render_targets;
	IRenderManager* _render_mgr;

	bool addRenderPipelines(DynamicLoader::ModulePtr& module);
	void generateCommandLists(double dt, void* frame_data);
	void renderToScreen(double dt, void* frame_data);

	SHIB_REF_DEF(RenderPipelineManager);
	REF_DEF_REQ;
};

NS_END
