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
#include <Shibboleth_SetupGraphicsState.h>
#include <Shibboleth_SetupOtterUIState.h>
#include <Shibboleth_Utilities.h>
#include <Shibboleth_IApp.h>
#include <Gaff_Image.h>
#include <Gaff_JSON.h>

#include <Shibboleth_RenderPipelineManager.h>
#include <Shibboleth_BulletPhysicsManager.h>
#include <Shibboleth_OcclusionManager.h>
#include <Shibboleth_CameraComponent.h>
#include <Shibboleth_UpdateManager.h>
#include <Shibboleth_ObjectManager.h>
#include <Shibboleth_RenderManager.h>
#include <Shibboleth_Object.h>

class LoopState : public Shibboleth::IState
{
public:
	LoopState(Shibboleth::IApp& app):
		_object(nullptr), _object2(nullptr),
		_camera(nullptr), _app(app)
	{
	}

	~LoopState(void)
	{
		if (_object) {
			Shibboleth::GetApp().getManagerT<Shibboleth::OcclusionManager>("Occlusion Manager").removeObject(_object);
			Shibboleth::GetApp().getManagerT<Shibboleth::ObjectManager>("Object Manager").removeObject(_object->getID());
		}

		if (_object2) {
			Shibboleth::GetApp().getManagerT<Shibboleth::OcclusionManager>("Occlusion Manager").removeObject(_object2);
			Shibboleth::GetApp().getManagerT<Shibboleth::ObjectManager>("Object Manager").removeObject(_object2->getID());
		}

		if (_camera) {
			Shibboleth::GetApp().getManagerT<Shibboleth::ObjectManager>("Object Manager").removeObject(_camera->getID());
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
		auto& phys_mgr = _app.getManagerT<Shibboleth::BulletPhysicsManager>();
		auto* plane_coll = phys_mgr.createCollisionShapeStaticPlane(0.0f, 1.0f, 0.0f, -15.0f);
		phys_mgr.addToMainWorld(phys_mgr.createRigidBody(nullptr, plane_coll, 0.0f));

		_app.getManagerT<Shibboleth::ResourceManager>("Resource Manager").addRequestAddedCallback(Gaff::Bind(this, &LoopState::ResReq));

		_object = _app.getManagerT<Shibboleth::ObjectManager>("Object Manager").createObject();

		if (_object) {
			if (_object->init("Resources/Objects/test.object")) {
				_object->setWorldPosition(Gleam::Vector4CPU(-2.5f, 0.0f, 0.0f, 1.0f));

			} else {
				_app.getManagerT<Shibboleth::ObjectManager>("Object Manager").removeObject(_object->getID());
				_object = nullptr;
				_app.quit();
			}

		} else {
			_app.quit();
		}

		_object2 = _app.getManagerT<Shibboleth::ObjectManager>("Object Manager").createObject();

		if (_object2) {
			if (_object2->init("Resources/Objects/test.object")) {
				_object2->setWorldPosition(Gleam::Vector4CPU(2.5f, 0.0f, 0.0f, 1.0f));

			} else {
				_app.getManagerT<Shibboleth::ObjectManager>("Object Manager").removeObject(_object2->getID());
				_object2 = nullptr;
				_app.quit();
			}

		} else {
			_app.quit();
		}

		_camera = _app.getManagerT<Shibboleth::ObjectManager>("Object Manager").createObject();

		if (_camera) {
			if (_camera->init("Resources/Objects/test_camera.object")) {
				_camera->setWorldPosition(Gleam::Vector4CPU(0.0f, 5.0f, -50.0f, 1.0f));
				
			} else {
				_app.getManagerT<Shibboleth::ObjectManager>("Object Manager").removeObject(_camera->getID());
				_camera = nullptr;
				_app.quit();
			}

		} else {
			_app.quit();
		}

		bool all_loaded = true;

		do {
			all_loaded = true;

			for (size_t i = 0; i < _resources.size(); ++i) {
				all_loaded &= _resources[i]->isLoaded();

				if (_resources[i]->hasFailed()) {
					all_loaded = true;
					_app.quit();
					break;
				}
			}
		} while (!all_loaded);

		_object->addToWorld();
		_object2->addToWorld();
		_camera->addToWorld();
	}

	void update(void)
	{
		if (!_object || !_object2 || !_camera || _app.isQuitting())
			return;

		static Shibboleth::CameraComponent* camera = _camera->getFirstComponentWithInterface<Shibboleth::CameraComponent>();
		static bool added = false;

		if (!added && camera && camera->getRenderTarget().getResourcePtr()->isLoaded()) {
			_app.getManagerT<Shibboleth::RenderPipelineManager>("Render Pipeline Manager").setOutputCamera(camera);
			added = true;
		}

		static Shibboleth::RenderManager& rm = _app.getManagerT<Shibboleth::RenderManager>("Render Manager");
		rm.updateWindows(); // This has to happen in the main thread.

		static Shibboleth::UpdateManager& update_manager = _app.getManagerT<Shibboleth::UpdateManager>("Update Manager");
		update_manager.update();

		YieldThread();
	}

	void exit(void)
	{
		_app.getManagerT<Shibboleth::ResourceManager>("Resource Manager").removeRequestAddedCallback(Gaff::Bind(this, &LoopState::ResReq));
		_resources.clear();
	}

private:
	Shibboleth::Object* _object;
	Shibboleth::Object* _object2;
	Shibboleth::Object* _camera;
	Shibboleth::IApp& _app;
	Shibboleth::Array<Shibboleth::ResourcePtr> _resources;
};

template <class State>
Shibboleth::IState* CreateStateT(Shibboleth::IApp& app)
{
	return Shibboleth::GetAllocator()->template allocT<State>(app);
}

enum States
{
	SETUP_GRAPHICS_STATE = 0,
	CREATE_RESOURCE_LOADERS_STATE,
	SETUP_OTTER_UI_STATE,
	LOAD_GRAPHICS_MODULE_STATE,
	LOOP_FOREVER_STATE,
	NUM_STATES
};

typedef Shibboleth::IState* (*CreateStateFunc)(Shibboleth::IApp&);

static CreateStateFunc create_funcs[] = {
	&CreateStateT<Shibboleth::SetupGraphicsState>,
	&CreateStateT<Shibboleth::CreateResourceLoadersState>,
	&CreateStateT<Shibboleth::SetupOtterUIState>,
	&CreateStateT<Shibboleth::LoadGraphicsModuleState>,
	&CreateStateT<LoopState>
};

static const char* state_names[NUM_STATES] = {
	"SetupGraphicsState",
	"CreateResourceLoadersState",
	"SetupOtterUIState",
	"LoadGraphicsModuleState",
	"loopforeverstate"
};

static size_t g_pool_index = 0;

void* MEMCB ImageAlloc(const size_t size)
{
	return Shibboleth::ShibbolethAllocate(size, g_pool_index);
}

void MEMCB ImageFree(const void* const data)
{
	Shibboleth::ShibbolethFree(const_cast<void*>(data));
}


DYNAMICEXPORT_C bool InitModule(Shibboleth::IApp& app)
{
	Gaff::JSON::SetHashSeed(app.getSeed());
	Shibboleth::SetApp(app);

	g_pool_index = Shibboleth::GetPoolIndex("Images");

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
	GAFF_ASSERT(id < NUM_STATES);
	return state_names[id];
}

DYNAMICEXPORT_C unsigned int GetNumStates(void)
{
	return NUM_STATES;
}

DYNAMICEXPORT_C Shibboleth::IState* CreateState(unsigned int id)
{
	GAFF_ASSERT(id < NUM_STATES);
	return create_funcs[id](Shibboleth::GetApp());
}

DYNAMICEXPORT_C void DestroyState(Shibboleth::IState* state, unsigned int)
{
	Shibboleth::GetAllocator()->freeT(state);
}
