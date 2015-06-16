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
#include "Shibboleth_IRenderPipeline.h"
#include <Shibboleth_IFileSystem.h>
#include <Shibboleth_Utilities.h>
#include <Shibboleth_IApp.h>

#include <Gaff_Utils.h>

NS_SHIBBOLETH

REF_IMPL_REQ(RenderPipelineManager);
SHIB_REF_IMPL(RenderPipelineManager)
.addBaseClassInterfaceOnly<RenderPipelineManager>()
.ADD_BASE_CLASS_INTERFACE_ONLY(IUpdateQuery)
;

RenderPipelineManager::RenderPipelineManager(void):
	_active_pipeline(SIZE_T_FAIL)
{
}

RenderPipelineManager::~RenderPipelineManager(void)
{
	for (auto it = _pipelines.begin(); it != _pipelines.end(); ++it) {
		_pipeline_map[(*it)->getName()](*it);
	}
}

const char* RenderPipelineManager::getName(void) const
{
	return "Render Pipeline Manager";
}

void RenderPipelineManager::allManagersCreated(void)
{
	LogManager::FileLockPair& log = GetApp().getGameLogFile();

	log.first.writeString("\n==================================================\n");
	log.first.writeString("==================================================\n");
	log.first.writeString("Loading Render Pipelines...\n");

	bool early_out = Gaff::ForEachTypeInDirectory<Gaff::FDT_RegularFile>("./Render Pipelines", [&](const char* name, size_t) -> bool
	{
		AString rel_path = AString("./Render Pipelines/") + name;

		// Error out if it's not a dynamic module
		if (!Gaff::File::checkExtension(name, DYNAMIC_EXTENSION)) {
			log.first.printf("ERROR - '%s' is not a dynamic module.\n", rel_path.getBuffer());
			GetApp().quit();
			return true;

		// It is a dynamic module, but not compiled for our architecture.
		// Or not compiled in our build mode. Just skip over it.
		} else if (!Gaff::File::checkExtension(name, BIT_EXTENSION DYNAMIC_EXTENSION)) {
			return false;
		}

		DynamicLoader::ModulePtr module = GetApp().loadModule(rel_path.getBuffer(), name);

		if (!module.valid()) {
			log.first.printf("ERROR - Could not load dynamic module '%s'.\n", rel_path.getBuffer());
			GetApp().quit();
			return true;
		}

		log.first.printf("Loading render pipelines from module '%s'\n", rel_path.getBuffer());

		if (!addRenderPipelines(module)) {
			log.first.printf("ERROR - Could not load render pipelines in dynamic module '%s'.\n", rel_path.getBuffer());
			GetApp().quit();
			return true;
		}

		return false;
	});

	if (early_out) {
		return;
	}

	IFileSystem* fs = GetApp().getFileSystem();
	IFile* file = fs->openFile("/graphics.cfg");

	if (!file) {
		log.first.printf("ERROR - Could not open file 'graphics.cfg'.\n");
		GetApp().quit();
		return;
	}

	Gaff::JSON config;

	if (!config.parse(file->getBuffer())) {
		log.first.printf("ERROR - 'graphics.cfg' is malformed. Could not parse file.\n");
		fs->closeFile(file);
		GetApp().quit();
		return;
	}

	fs->closeFile(file);

	if (!config.isObject()) {
		log.first.printf("ERROR - 'graphics.cfg' is malformed. Root element is not an object.\n");
		GetApp().quit();
		return;
	}

	Gaff::JSON initial_pipeline = config["initial_pipeline"];

	if (!initial_pipeline.isString()) {
		log.first.printf("ERROR - 'graphics.cfg' is malformed. Element at 'initial_pipeline' is not a string.\n");
		GetApp().quit();
		return;
	}

	for (size_t i = 0; i < _pipelines.size(); ++i) {
		if (!strcmp(_pipelines[i]->getName(), initial_pipeline.getString())) {
			_active_pipeline = i;
			break;
		}
	}

	if (_active_pipeline == SIZE_T_FAIL) {
		log.first.printf("ERROR - Could not find initial render pipeline '%s'.\n", initial_pipeline.getString());
		GetApp().quit();
	}
}

void RenderPipelineManager::getUpdateEntries(Array<UpdateEntry>& entries)
{
	entries.emplacePush(AString("Render Pipeline Manager: Generate Command Lists"), Gaff::Bind(this, &RenderPipelineManager::generateCommandLists));
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
		if (_pipelines[i]->getName() == name) {
			return i;
		}
	}

	return SIZE_T_FAIL;
}

bool RenderPipelineManager::addRenderPipelines(DynamicLoader::ModulePtr& module)
{
	LogManager::FileLockPair& log = GetApp().getGameLogFile();

	assert(module.valid());
	CreateRenderPipelineFunc create_pipeline = module->GetFunc<CreateRenderPipelineFunc>("CreateRenderPipeline");
	DestroyRenderPipelineFunc destroy_pipeline = module->GetFunc<DestroyRenderPipelineFunc>("DestroyRenderPipeline");
	GetNumRenderPipelinesFunc num_pipelines = module->GetFunc<GetNumRenderPipelinesFunc>("GetNumRenderPipelines");

	InitFunc init_func = module->GetFunc<InitFunc>("InitModule");

	if (!init_func) {
		log.first.writeString("ERROR - Could not find function named 'InitModule'.\n");
		return false;
	}

	if (!create_pipeline) {
		log.first.writeString("ERROR - Could not find function named 'CreateRenderPipeline'.\n");
		return false;
	}

	if (!destroy_pipeline) {
		log.first.writeString("ERROR - Could not find function named 'DestroyRenderPipeline'.\n");
		return false;
	}

	if (!num_pipelines) {
		log.first.writeString("ERROR - Could not find function named 'GetNumRenderPipelines'.\n");
		return false;
	}

	if (!init_func(Shibboleth::GetApp())) {
		log.first.writeString("ERROR - Failed to initialize render pipeline module.\n");
	}

	for (size_t i = 0; i < num_pipelines(); ++i) {
		IRenderPipeline* pipeline = create_pipeline(i);

		if (!pipeline) {
			log.first.printf("ERROR - Failed to create pipeline with ID '%i'", i);
			return false;
		}

		_pipeline_map[pipeline->getName()] = destroy_pipeline;
		_pipelines.push(pipeline);

		log.first.printf("Loaded render pipeline '%s'\n", pipeline->getName());
	}

	return true;
}

void RenderPipelineManager::generateCommandLists(double dt, void* frame_data)
{
	//_pipelines[_active_pipeline]->run(dt, frame_data);
}

NS_END
