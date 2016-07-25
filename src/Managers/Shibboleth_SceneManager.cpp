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
#include "Shibboleth_ISchemaManager.h"
#include "Shibboleth_IObjectManager.h"
#include <Shibboleth_IFileSystem.h>
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

void LoadSceneJob(void* data)
{
	SceneManager* sm = reinterpret_cast<SceneManager*>(data);
	IApp& app = GetApp();
	IFileSystem* fs = app.getFileSystem();
	IFile* file = fs->openFile(sm->_scene_path.getBuffer());

	if (!file) {
		// log error
		// set to failure state
		return;
	}

	Gaff::JSON scene_data;

	if (!scene_data.parse(file->getBuffer())) {
		fs->closeFile(file);
		// log error
		// set to failure state
		return;
	}

	fs->closeFile(file);

	Gaff::JSON schema = app.getManagerT<ISchemaManager>().getSchema("Scene.schema");

	if (!scene_data.validate(schema)) {
		// log error
		// set to failure state
		return;
	}

	Gaff::JSON layers = scene_data["Layers"];

	sm->_scene.name = scene_data["Name"].getString();
	sm->_scene.layers.resize(layers.size());

	layers.forEachInArray([sm](size_t index, const Gaff::JSON& value) -> bool
	{
		sm->loadLayer(index, value);
		return false;
	});
}

REF_IMPL_REQ(SceneManager);
SHIB_REF_IMPL(SceneManager)
.addBaseClassInterfaceOnly<SceneManager>()
.ADD_BASE_CLASS_INTERFACE_ONLY(ISceneManager)
;

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

void SceneManager::loadScene(const char* scene)
{
	_scene_path = scene;
	Gaff::JobData job_data(LoadSceneJob, this);
	GetApp().getJobPool().addJobs(&job_data);
}

bool SceneManager::activateLayer(const char* layer)
{
	uint32_t hash = Gaff::FNV1aHash32(layer, strlen(layer));

	for (size_t i = 0; i < _scene.layers.size(); ++i) {
		if (_scene.layers[i].hash == hash) {
			activateLayer(i);
			return true;
		}
	}

	return false;
}

bool SceneManager::deactivateLayer(const char* layer)
{
	uint32_t hash = Gaff::FNV1aHash32(layer, strlen(layer));

	for (size_t i = 0; i < _scene.layers.size(); ++i) {
		if (_scene.layers[i].hash == hash) {
			deactivateLayer(i);
			return true;
		}
	}

	return false;
}

void SceneManager::activateLayer(size_t layer)
{
	GAFF_ASSERT(layer < _scene.layers.size());
	auto& objects = _scene.layers[layer].objects;

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
	GAFF_ASSERT(layer < _scene.layers.size());
	auto& objects = _scene.layers[layer].objects;

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

void SceneManager::loadLayer(size_t index, const Gaff::JSON& layer_data)
{
	Layer& layer = _scene.layers[index];
	layer.name = layer_data["Name"].getString();
	bool active = layer_data["Active"].getBool();

	IApp& app = GetApp();
	IFileSystem* fs = app.getFileSystem();
	IFile* file = fs->openFile(layer_data["Layer File"].getString());

	if (!file) {
		// log error
		return;
	}

	Gaff::JSON lyr;

	if (!lyr.parse(file->getBuffer())) {
		fs->closeFile(file);

		app.getLogManager().logMessage(
			LogManager::LOG_ERROR, app.getLogFileName(),
			"Failed to parse layer file '%s' with error '%s'.",
			layer_data["Layer File"].getString(),
			lyr.getErrorText()
		);

		return;
	}

	fs->closeFile(file);

	Gaff::JSON schema = app.getManagerT<ISchemaManager>().getSchema("Layer.schema");

	if (!lyr.validate(schema)) {
		// log error
		return;
	}

	IObjectManager& om = app.getManagerT<IObjectManager>();
	layer.objects.resize(lyr.size(), nullptr);

	lyr.forEachInArray([&layer, &om, fs](size_t index, const Gaff::JSON& value) -> bool
	{
		Gaff::JSON obj_file = value["Object File"];

		Object* object = om.createObject();
		bool success = true;

		if (!obj_file.isNull()) {
			success = object->init(fs, obj_file.getString());
		} else {
			success = object->init(value["Object"]);
		}

		if (success) {
			layer.objects[index] = object;

			Gaff::JSON scale = value["Scale"];
			Gaff::JSON rot = value["Rotation"];
			Gaff::JSON pos = value["Position"];

			Gleam::TransformCPU transform;
			Gleam::QuaternionCPU r;
			Gleam::Vector4CPU s, p;

			s.set(
				static_cast<float>(scale[static_cast<size_t>(0)].getDouble()),
				static_cast<float>(scale[static_cast<size_t>(1)].getDouble()),
				static_cast<float>(scale[static_cast<size_t>(2)].getDouble()),
				1.0f
			);

			r.set(
				static_cast<float>(rot[static_cast<size_t>(0)].getDouble()),
				static_cast<float>(rot[static_cast<size_t>(1)].getDouble()),
				static_cast<float>(rot[static_cast<size_t>(2)].getDouble()),
				static_cast<float>(rot[static_cast<size_t>(3)].getDouble())
			);

			p.set(
				static_cast<float>(pos[static_cast<size_t>(0)].getDouble()),
				static_cast<float>(pos[static_cast<size_t>(1)].getDouble()),
				static_cast<float>(pos[static_cast<size_t>(2)].getDouble()),
				1.0f
			);

			transform.setTranslation(p);
			transform.setRotation(r);
			transform.setScale(s);

			object->setLocalTransform(transform);

		} else {
			// log error
			om.removeObject(object);
		}

		return false;
	});

	// Run it through again to hook up the children
	lyr.forEachInArray([&layer](size_t parent_index, const Gaff::JSON& object) -> bool
	{
		Object* parent = layer.objects[parent_index];

		object["Children"].forEachInArray([&layer, parent](size_t, const Gaff::JSON& child_index) -> bool
		{
			Object* child = layer.objects[child_index.getUInt()];
			parent->addChild(child);
			return false;
		});

		return false;
	});

	// If the layer starts out active, add the objects to the world
	if (active) {
		for (auto it = layer.objects.begin(); it != layer.objects.end(); ++it) {
			(*it)->addToWorld();
		}
	}

}

NS_END
