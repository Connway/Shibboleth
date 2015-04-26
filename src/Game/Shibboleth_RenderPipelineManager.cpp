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

#include "Shibboleth_RenderPipelineManager.h"
#include "Shibboleth_RenderPipeline.h"
#include <Shibboleth_TaskPoolTags.h>
#include <Shibboleth_IFileSystem.h>
#include <Shibboleth_Utilities.h>
#include <Shibboleth_App.h>
#include <Gaff_Utils.h>

NS_SHIBBOLETH

REF_IMPL_REQ(RenderPipelineManager);
SHIB_REF_IMPL(RenderPipelineManager)
.addBaseClassInterfaceOnly<RenderPipelineManager>()
.ADD_BASE_CLASS_INTERFACE_ONLY(IUpdateQuery)
.ADD_BASE_CLASS_INTERFACE_ONLY(IRenderStageQuery)
;

RenderPipelineManager::RenderPipelineManager(void):
	_active_pipeline(0)
{
}

RenderPipelineManager::~RenderPipelineManager(void)
{
}

const char* RenderPipelineManager::getName(void) const
{
	return "Render Pipeline Manager";
}

void RenderPipelineManager::allManagersCreated(void)
{
	unsigned int render_hash = CLASS_HASH(IRenderStageQuery);
	Array<IRenderStageQuery::RenderStageEntry> entries;

	reinterpret_cast<App&>(GetApp()).forEachManager([&](IManager& manager) -> bool
	{
		IRenderStageQuery* render_query = manager.requestInterface<IRenderStageQuery>(render_hash);

		if (render_query) {
			render_query->getRenderStageEntries(entries);
		}

		return false;
	});

	// Loop through all assets in Render Pipelines and read them.
	IFile* file = GetApp().getFileSystem()->openFile("Resources/Render Pipelines/Pipelines.json");

	if (!file) {
		LogMessage(GetApp().getGameLogFile(), TPT_PRINTLOG, LogManager::LOG_ERROR, "ERROR - Failed to open file 'Resources/Render Pipelines/Pipelines.json'.\n");
		GetApp().quit();
		return;
	}

	Gaff::JSON pipelines;

	if (!pipelines.parse(file->getBuffer())) {
		LogMessage(GetApp().getGameLogFile(), TPT_PRINTLOG, LogManager::LOG_ERROR, "ERROR - Failed to parse file 'Resources/Render Pipelines/Pipelines.json'.\n");
		GetApp().quit();
		return;
	}

	GetApp().getFileSystem()->closeFile(file);

	if (!pipelines.isArray()) {
		LogMessage(GetApp().getGameLogFile(), TPT_PRINTLOG, LogManager::LOG_ERROR, "ERROR - Root element in 'Resources/Render Pipelines/Pipelines.json' is not an array.\n");
		GetApp().quit();
		return;
	}

	bool failed = pipelines.forEachInArray([&](size_t, const Gaff::JSON& pline_file) -> bool
	{
		if (!pline_file.isString()) {
			LogMessage(GetApp().getGameLogFile(), TPT_PRINTLOG, LogManager::LOG_ERROR, "ERROR - Element in 'Resources/Render Pipelines/Pipelines.json' is not a string.\n");
			return true;
		}

		file = GetApp().getFileSystem()->openFile(pline_file.getString());

		if (!file) {
			LogMessage(GetApp().getGameLogFile(), TPT_PRINTLOG, LogManager::LOG_ERROR, "ERROR - Failed to open file '%s'.\n", pline_file.getString());
			return true;
		}

		Gaff::JSON stages;
		
		if (!stages.parse(file->getBuffer())) {
			LogMessage(GetApp().getGameLogFile(), TPT_PRINTLOG, LogManager::LOG_ERROR, "ERROR - Failed to parse file '%s'.\n", pline_file.getString());
			GetApp().getFileSystem()->closeFile(file);
			return true;
		}

		GetApp().getFileSystem()->closeFile(file);

		if (!stages.isArray()) {
			LogMessage(GetApp().getGameLogFile(), TPT_PRINTLOG, LogManager::LOG_ERROR, "ERROR - Root element in '%s' is not an array.\n", pline_file.getString());
			return true;
		}

		_pipelines.resize(stages.size());
		size_t index = 0;

		bool early_out = stages.forEachInArray([&](size_t, Gaff::JSON pline) -> bool
		{
			if (!pline.isString()) {
				LogMessage(GetApp().getGameLogFile(), TPT_PRINTLOG, LogManager::LOG_ERROR, "ERROR - Element in '%s' is not a string.\n", pline.getString());
				return true;
			}

			file = GetApp().getFileSystem()->openFile(pline.getString());

			if (!file) {
				LogMessage(GetApp().getGameLogFile(), TPT_PRINTLOG, LogManager::LOG_ERROR, "ERROR - Failed to open file '%s'.\n", pline.getString());
				return true;
			}

			Gaff::JSON pipeline;
			
			if (!pipeline.parse(file->getBuffer())) {
				LogMessage(GetApp().getGameLogFile(), TPT_PRINTLOG, LogManager::LOG_ERROR, "ERROR - Failed to parse file '%s'.\n", pline_file.getString());
				GetApp().getFileSystem()->closeFile(file);
				return true;
			}

			GetApp().getFileSystem()->closeFile(file);

			if (!pipeline.isObject()) {
				LogMessage(GetApp().getGameLogFile(), TPT_PRINTLOG, LogManager::LOG_ERROR, "ERROR - Root element in '%s' is not an object.\n", pipeline.getString());
				return true;
			}

			LogMessage(GetApp().getGameLogFile(), TPT_PRINTLOG, LogManager::LOG_ERROR, "Parsing render pipeline '%s'.\n", pipeline.getString());

			if (!_pipelines[index].init(pipeline, _stages)) {
				return true;
			}

			return true;
		});

		if (early_out) {
			GetApp().quit();
			return true;
		}

		return false;
	});

	if (failed) {
		GetApp().quit();
		return;
	}
}

void RenderPipelineManager::getUpdateEntries(Array<UpdateEntry>& entries)
{
	entries.emplacePush(AString("Render Pipeline Manager: Update"), Gaff::Bind(this, &RenderPipelineManager::update));
}

void RenderPipelineManager::getRenderStageEntries(Array<RenderStageEntry>& entries)
{
}

void RenderPipelineManager::setOutputCamera(unsigned int monitor, CameraComponent* camera)
{
	assert(monitor < _output_cameras.size());
	_output_cameras[monitor] = camera;
}

CameraComponent* RenderPipelineManager::getOutputCamera(unsigned int monitor) const
{
	assert(monitor < _output_cameras.size());
	return _output_cameras[monitor];
}

void RenderPipelineManager::setNumMonitors(unsigned int num_monitors)
{
	_output_cameras.resize(num_monitors);
}

size_t RenderPipelineManager::getActivePipeline(void) const
{
	return _active_pipeline;
}

void RenderPipelineManager::setActivePipeline(size_t pipeline)
{
	assert(pipeline < _pipelines.size());
	_active_pipeline = pipeline;
}

size_t RenderPipelineManager::getPipelineIndex(const char* name) const
{
	for (size_t i = 0; i < _pipelines.size(); ++i) {
		if (_pipelines[i].getName() == name) {
			return i;
		}
	}

	return SIZE_T_FAIL;
}

void RenderPipelineManager::update(double)
{
	_pipelines[_active_pipeline].run();
}

NS_END
