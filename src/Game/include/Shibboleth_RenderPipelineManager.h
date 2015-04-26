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

#include "Shibboleth_IRenderStageQuery.h"
#include "Shibboleth_IUpdateQuery.h"
#include <Shibboleth_ReflectionDefinitions.h>
#include <Shibboleth_IManager.h>

NS_SHIBBOLETH

class CameraComponent;
class RenderPipeline;

class RenderPipelineManager : public IManager, public IUpdateQuery, public IRenderStageQuery
{
public:
	RenderPipelineManager(void);
	~RenderPipelineManager(void);

	const char* getName(void) const;
	void allManagersCreated(void);

	void* rawRequestInterface(unsigned int class_id) const;
	void getUpdateEntries(Array<UpdateEntry>& entries);

	void getRenderStageEntries(Array<RenderStageEntry>& entries);

	INLINE void setOutputCamera(unsigned int monitor, CameraComponent* camera);
	INLINE CameraComponent* getOutputCamera(unsigned int monitor) const;
	INLINE void setNumMonitors(unsigned int num_monitors);

	INLINE size_t getActivePipeline(void) const;
	INLINE void setActivePipeline(size_t pipeline);

	size_t getPipelineIndex(const char* name) const;

	void update(double);

private:
	Array<CameraComponent*> _output_cameras; // Array size == num of monitors. Element is camera that is being outputted to monitor.
	Array<RenderPipeline> _pipelines;
	Array<IRenderStageQuery::RenderStageEntry> _stages;
	size_t _active_pipeline;

	void renderOpaque(void);
	void composeImage(void);
	void renderTransparent(void);
	void renderToScreen(void);

	SHIB_REF_DEF(RenderPipelineManager);
};

NS_END
