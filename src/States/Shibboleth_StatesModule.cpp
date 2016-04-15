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

#include <Shibboleth_CreateResourceLoadersState.h>
#include <Shibboleth_LoadGraphicsModuleState.h>
#include <Shibboleth_SetupDevicesState.h>
#include <Shibboleth_SetupOtterUIState.h>

#include <Shibboleth_ModuleHelpers.h>
#include <Shibboleth_Utilities.h>
#include <Shibboleth_IApp.h>
#include <Gaff_Image.h>
#include <Gaff_JSON.h>

#include <Shibboleth_RenderPipelineManager.h>
#include <Shibboleth_OcclusionManager.h>
#include <Shibboleth_CameraComponent.h>
#include <Shibboleth_UpdateManager.h>
#include <Shibboleth_ObjectManager.h>
#include <Shibboleth_RenderManager.h>
#include <Shibboleth_Object.h>

class LoopState : public Shibboleth::IState
{
public:
	static const char* GetFriendlyName(void)
	{
		return "Loop State";
	}

	LoopState(void):
		_object(nullptr), _object2(nullptr),
		_camera(nullptr), _floor(nullptr)
	{
	}

	~LoopState(void)
	{
		auto& occ_mgr = Shibboleth::GetApp().getManagerT<Shibboleth::OcclusionManager>("Occlusion Manager");
		auto& obj_mgr = Shibboleth::GetApp().getManagerT<Shibboleth::ObjectManager>("Object Manager");

		if (_object) {
			occ_mgr.removeObject(_object);
			obj_mgr.removeObject(_object->getID());
		}

		if (_object2) {
			occ_mgr.removeObject(_object2);
			obj_mgr.removeObject(_object2->getID());
		}

		if (_camera) {
			obj_mgr.removeObject(_camera->getID());
		}

		if (_floor) {
			obj_mgr.removeObject(_floor->getID());
		}
	}

	void ResReq(Shibboleth::ResourcePtr& res)
	{
		_resources.emplacePush(res);
	}

	bool init(unsigned int)
	{
		return true;
	}

	void enter(void)
	{
		Shibboleth::IApp& app = Shibboleth::GetApp();

		app.getManagerT<Shibboleth::ResourceManager>("Resource Manager").addRequestAddedCallback(Gaff::Bind(this, &LoopState::ResReq));

		_object = app.getManagerT<Shibboleth::ObjectManager>("Object Manager").createObject();

		if (_object) {
			if (_object->init("Resources/Objects/test.object")) {
				_object->setWorldPosition(Gleam::Vector4CPU(-2.5f, 0.0f, 0.0f, 1.0f));

			} else {
				app.getManagerT<Shibboleth::ObjectManager>("Object Manager").removeObject(_object->getID());
				_object = nullptr;
				app.quit();
			}

		} else {
			app.quit();
		}

		_object2 = app.getManagerT<Shibboleth::ObjectManager>("Object Manager").createObject();

		if (_object2) {
			if (_object2->init("Resources/Objects/test.object")) {
				_object2->setWorldPosition(Gleam::Vector4CPU(2.5f, 0.0f, 0.0f, 1.0f));

			} else {
				app.getManagerT<Shibboleth::ObjectManager>("Object Manager").removeObject(_object2->getID());
				_object2 = nullptr;
				app.quit();
			}

		} else {
			app.quit();
		}

		_camera = app.getManagerT<Shibboleth::ObjectManager>("Object Manager").createObject();

		if (_camera) {
			if (_camera->init("Resources/Objects/test_camera.object")) {
				_camera->setWorldPosition(Gleam::Vector4CPU(0.0f, 5.0f, -50.0f, 1.0f));

			} else {
				app.getManagerT<Shibboleth::ObjectManager>("Object Manager").removeObject(_camera->getID());
				_camera = nullptr;
				app.quit();
			}

		} else {
			app.quit();
		}

		_floor = app.getManagerT<Shibboleth::ObjectManager>("Object Manager").createObject();

		if (_floor) {
			if (!_floor->init("Resources/Objects/floor.object")) {
				app.getManagerT<Shibboleth::ObjectManager>("Object Manager").removeObject(_floor->getID());
				_floor = nullptr;
				app.quit();
			}

		} else {
			app.quit();
		}

		bool all_loaded = true;

		do {
			all_loaded = true;

			for (size_t i = 0; i < _resources.size(); ++i) {
				all_loaded &= _resources[i]->isLoaded();

				if (_resources[i]->hasFailed()) {
					all_loaded = true;
					app.quit();
					break;
				}
			}
		} while (!all_loaded);

		_object->addToWorld();
		_object2->addToWorld();
		_camera->addToWorld();
		_floor->addToWorld();
	}

	void update(void)
	{
		Shibboleth::IApp& app = Shibboleth::GetApp();

		if (!_object || !_object2 || !_camera || app.isQuitting())
			return;

		static Shibboleth::CameraComponent* camera = _camera->getFirstComponentWithInterface<Shibboleth::CameraComponent>();
		static bool added = false;

		if (!added && camera && camera->getRenderTarget().getResourcePtr()->isLoaded()) {
			app.getManagerT<Shibboleth::RenderPipelineManager>("Render Pipeline Manager").setOutputCamera(camera);
			added = true;
		}

		static Shibboleth::RenderManager& rm = app.getManagerT<Shibboleth::RenderManager>("Render Manager");
		rm.updateWindows(); // This has to happen in the main thread.

		static Shibboleth::UpdateManager& update_manager = app.getManagerT<Shibboleth::UpdateManager>("Update Manager");
		update_manager.update();

		YieldThread();
	}

	void exit(void)
	{
		Shibboleth::IApp& app = Shibboleth::GetApp();
		app.getManagerT<Shibboleth::ResourceManager>("Resource Manager").removeRequestAddedCallback(Gaff::Bind(this, &LoopState::ResReq));
		_resources.clear();
	}

private:
	Shibboleth::Object* _object;
	Shibboleth::Object* _object2;
	Shibboleth::Object* _camera;
	Shibboleth::Object* _floor;
	Shibboleth::Array<Shibboleth::ResourcePtr> _resources;
};

static Shibboleth::StateFuncs states[] = {
	{ Shibboleth::CreateStateT<Shibboleth::SetupDevicesState>, Shibboleth::SetupDevicesState::GetFriendlyName },
	{ Shibboleth::CreateStateT<Shibboleth::CreateResourceLoadersState>, Shibboleth::CreateResourceLoadersState::GetFriendlyName },
	{ Shibboleth::CreateStateT<Shibboleth::SetupOtterUIState>, Shibboleth::SetupOtterUIState::GetFriendlyName },
	{ Shibboleth::CreateStateT<Shibboleth::LoadGraphicsModuleState>, Shibboleth::LoadGraphicsModuleState::GetFriendlyName },
	{ Shibboleth::CreateStateT<LoopState>, LoopState::GetFriendlyName }
};

static size_t g_image_pool_index = 0;

void* MEMCB ImageAlloc(const size_t size)
{
	return Shibboleth::ShibbolethAllocate(size, g_image_pool_index);
}

void MEMCB ImageFree(const void* const data)
{
	Shibboleth::ShibbolethFree(const_cast<void*>(data));
}


DYNAMICEXPORT_C bool InitModule(Shibboleth::IApp& app)
{
	Gaff::JSON::SetHashSeed(app.getSeed());
	Shibboleth::SetApp(app);

	g_image_pool_index = Shibboleth::GetPoolIndex("Images");

	Gaff::Image::SetMemoryFunctions(ImageAlloc, ImageFree);
	Gaff::Image::SysInit();

	return true;
}

DYNAMICEXPORT_C void ShutdownModule(void)
{
	Gaff::Image::SysShutdown();
}

DYNAMICEXPORT_C const char* GetStateName(unsigned int id)
{
	GAFF_ASSERT(id < ARRAY_SIZE(states));
	return states[id].name();
}

DYNAMICEXPORT_C unsigned int GetNumStates(void)
{
	return ARRAY_SIZE(states);
}

DYNAMICEXPORT_C Shibboleth::IState* CreateState(unsigned int id)
{
	GAFF_ASSERT(id < ARRAY_SIZE(states));
	return states[id].create();
}

DYNAMICEXPORT_C void DestroyState(Shibboleth::IState* state, unsigned int)
{
	SHIB_FREET(state, *Shibboleth::GetAllocator());
}
