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

#include <Shibboleth_IRenderPipelineManager.h>
#include <Shibboleth_IOcclusionManager.h>
#include <Shibboleth_ICameraComponent.h>
#include <Shibboleth_IUpdateManager.h>
#include <Shibboleth_IObjectManager.h>
#include <Shibboleth_ISceneManager.h>
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
		auto& occ_mgr = Shibboleth::GetApp().getManagerT<Shibboleth::IOcclusionManager>();
		auto& obj_mgr = Shibboleth::GetApp().getManagerT<Shibboleth::IObjectManager>();

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
		Gaff::ScopedLock<Gaff::SpinLock> lock(_res_lock);
		_resources.emplacePush(res);
	}

	bool init(unsigned int)
	{
		return true;
	}

	void enter(void)
	{
		Shibboleth::IApp& app = Shibboleth::GetApp();
		app.getManagerT<Shibboleth::IResourceManager>().addRequestAddedCallback(Gaff::Bind(this, &LoopState::ResReq));
		app.getManagerT<Shibboleth::ISceneManager>().loadScene("Resources/Scenes/test.scene");

	}

	void update(void)
	{
		Shibboleth::IApp& app = Shibboleth::GetApp();

		bool all_loaded = true;

		do {
			all_loaded = true;

			Gaff::ScopedLock<Gaff::SpinLock> lock(_res_lock);

			for (size_t i = 0; i < _resources.size(); ++i) {
				all_loaded &= _resources[i]->isLoaded();

				if (_resources[i]->hasFailed()) {
					all_loaded = true;
					app.quit();
					return;
				}
			}
		} while (!all_loaded || _resources.empty());

		static bool already_set = false;

		if (!already_set) {
			Shibboleth::Object* camera = app.getManagerT<Shibboleth::IObjectManager>().findObject("Test Camera");

			if (camera) {
				Shibboleth::ICameraComponent* camera_cmp = camera->getFirstComponentWithInterface<Shibboleth::ICameraComponent>();

				if (camera_cmp) {
					app.getManagerT<Shibboleth::IRenderPipelineManager>().setOutputCamera(camera_cmp);
					already_set = true;
				}
			}
		}

		static Shibboleth::IRenderManager& rm = app.getManagerT<Shibboleth::IRenderManager>();
		rm.updateWindows(); // This has to happen in the main thread.

		static Shibboleth::IUpdateManager& update_manager = app.getManagerT<Shibboleth::IUpdateManager>();
		update_manager.update();

		YieldThread();
	}

	void exit(void)
	{
		Shibboleth::IApp& app = Shibboleth::GetApp();
		app.getManagerT<Shibboleth::IResourceManager>().removeRequestAddedCallback(Gaff::Bind(this, &LoopState::ResReq));
		_resources.clear();
	}

private:
	Shibboleth::Object* _object;
	Shibboleth::Object* _object2;
	Shibboleth::Object* _camera;
	Shibboleth::Object* _floor;
	Shibboleth::Array<Shibboleth::ResourcePtr> _resources;
	Gaff::SpinLock _res_lock;
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
