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

#include "Shibboleth_RenderManager.h"
#include "Shibboleth_IUpdateQuery.h"
#include <Shibboleth_ReflectionDefinitions.h>
#include <Shibboleth_ResourceDefines.h>

NS_SHIBBOLETH

class CameraComponent;
class IRenderPipeline;

class RenderPipelineManager : public IManager, public IUpdateQuery
{
public:
	RenderPipelineManager(void);
	~RenderPipelineManager(void);

	const char* getName(void) const;
	void allManagersCreated(void);

	void* rawRequestInterface(unsigned int class_id) const;
	void getUpdateEntries(Array<UpdateEntry>& entries);

	INLINE void setOutputCamera(unsigned int monitor, CameraComponent* camera);
	INLINE CameraComponent* getOutputCamera(unsigned int monitor) const;
	INLINE void setNumMonitors(unsigned int num_monitors);

	INLINE size_t getActivePipeline(void) const;
	INLINE void setActivePipeline(size_t pipeline);

	size_t getPipelineIndex(const char* name) const;

private:
	using InitFunc = bool (*)(IApp&);
	using CreateRenderPipelineFunc = IRenderPipeline* (*)(size_t);
	using DestroyRenderPipelineFunc = void (*)(IRenderPipeline*);
	using GetNumRenderPipelinesFunc = size_t (*)(void);

	Array< Gaff::Pair<CameraComponent*, unsigned int> > _output_cameras; // Array size == num of monitors. Element is camera that is being outputted to monitor and device id.
	Array<IRenderPipeline*> _pipelines;
	HashMap<AHashString, DestroyRenderPipelineFunc> _pipeline_map;
	size_t _active_pipeline;

	RenderManager::WindowRenderTargets _render_targets;
	RenderManager* _render_mgr;

	bool addRenderPipelines(DynamicLoader::ModulePtr& module);
	void generateCommandLists(double dt, void* frame_data);
	void renderToScreen(double dt, void* frame_data);

	SHIB_REF_DEF(RenderPipelineManager);
};

NS_END
