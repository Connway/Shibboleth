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

#include "Shibboleth_SceneManager.h"
#include <Shibboleth_Utilities.h>
#include <Shibboleth_Object.h>
#include <Shibboleth_IApp.h>

NS_SHIBBOLETH

void AddToWorldJob(void* data)
{
	Object* object = reinterpret_cast<Object*>(data);
	object->addToWorld();
}

void RemoveFromWorldJob(void* data)
{
	Object* object = reinterpret_cast<Object*>(data);
	object->removeFromWorld();
}

REF_IMPL_REQ(SceneManager);
SHIB_REF_IMPL(SceneManager)
.addBaseClassInterfaceOnly<SceneManager>()
;

const char* SceneManager::GetFriendlyName(void)
{
	return "Scene Manager";
}

SceneManager::SceneManager(void)
{
}

SceneManager::~SceneManager(void)
{
}

const char* SceneManager::getName(void) const
{
	return GetFriendlyName();
}

bool SceneManager::loadScene(const char* /*scene_name*/)
{
	// open file
	// parse JSON

	return true;
}

void SceneManager::activateLayer(size_t layer)
{
	GAFF_ASSERT(layer < _layers.size());
	auto& objects = _layers[layer].objects;

	auto& job_data = _job_cache[0][layer];

	for (auto it = objects.begin(); it != objects.end(); ++it) {
		if (!(*it)->isInWorld()) {
			job_data.emplacePush(AddToWorldJob, *it);
			//(*it)->addToWorld();
		}
	}

	GetApp().getJobPool().addJobs(job_data.getArray(), job_data.size());
	job_data.clearNoFree();
}

void SceneManager::deactivateLayer(size_t layer)
{
	GAFF_ASSERT(layer < _layers.size());
	auto& objects = _layers[layer].objects;

	auto& job_data = _job_cache[1][layer];

	for (auto it = objects.begin(); it != objects.end(); ++it) {
		if ((*it)->isInWorld()) {
			job_data.emplacePush(RemoveFromWorldJob, *it);
			//(*it)->removeFromWorld();
		}
	}

	GetApp().getJobPool().addJobs(job_data.getArray(), job_data.size());
	job_data.clearNoFree();
}

NS_END
