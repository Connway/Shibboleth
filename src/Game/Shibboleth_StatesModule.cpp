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

#include <Shibboleth_CreateResourceLoadersState.h>
#include <Shibboleth_LoadComponentsState.h>
#include <Shibboleth_SetupGraphicsState.h>
#include <Shibboleth_SetupOtterUIState.h>
#include <Shibboleth_Utilities.h>
#include <Shibboleth_IApp.h>
#include <Gleam_Global.h>
#include <Gaff_Image.h>
#include <Gaff_JSON.h>

#include <Shibboleth_OcclusionManager.h>
#include <Shibboleth_ModelComponent.h>
#include <Shibboleth_UpdateManager.h>
#include <Shibboleth_ObjectManager.h>
#include <Shibboleth_RenderManager.h>
#include <Shibboleth_Object.h>

#include <Gleam_IRenderDevice.h>

#ifdef USE_VLD
	#include <vld.h>
#endif

class LoopState : public Shibboleth::IState
{
public:
	LoopState(Shibboleth::IApp& app) : _object(nullptr), _app(app) {}
	~LoopState(void)
	{
		if (_object) {
			Shibboleth::GetApp().getManagerT<Shibboleth::OcclusionManager>("Occlusion Manager").removeObject(_object);
			Shibboleth::GetApp().getManagerT<Shibboleth::ObjectManager>("Object Manager").removeObject(_object->getID());
		}
	}

	bool init(unsigned int)
	{
		return true;
	}

	void enter(void)
	{
		_object = _app.getManagerT<Shibboleth::ObjectManager>("Object Manager").createObject();

		if (_object) {
			if (_object->init("Resources/Objects/test.object")) {
				_app.getManagerT<Shibboleth::OcclusionManager>("Occlusion Manager").addObject(_object, Shibboleth::OcclusionManager::OT_DYNAMIC);
			} else {
				_app.getManagerT<Shibboleth::ObjectManager>("Object Manager").removeObject(_object->getID());
				_app.quit();
				_object = nullptr;
			}

		} else {
			_app.quit();
		}

		//Shibboleth::RenderManager& rm = _app.getManagerT<Shibboleth::RenderManager>("Render Manager");
		//rm.addRenderFunction(Gaff::Bind(this, &LoopState::render));
	}

	void update(void)
	{
		if (!_object)
			return;

		Shibboleth::RenderManager& rm = _app.getManagerT<Shibboleth::RenderManager>("Render Manager");
		rm.updateWindows(); // This has to happen in the main thread.

		Shibboleth::UpdateManager& update_manager = _app.getManagerT<Shibboleth::UpdateManager>("Update Manager");
		update_manager.update(0.0f);

		render();
	}

	void exit(void)
	{
	}

	void render(void)
	{
		Shibboleth::ModelComponent* model = _object->getFirstComponentWithInterface<Shibboleth::ModelComponent>();
		Shibboleth::RenderManager& rm = _app.getManagerT<Shibboleth::RenderManager>("Render Manager");

		rm.getSpinLock().lock(); // Have to lock just in case other resources are trying to load and use the device
		rm.getRenderDevice().setCurrentDevice(0);
		rm.getRenderDevice().beginFrame();

		if (model) {
			model->render();
		}

		rm.getRenderDevice().endFrame();
		rm.getSpinLock().unlock();
	}

private:
	Shibboleth::Object* _object;
	Shibboleth::IApp& _app;
};

template <class State>
Shibboleth::IState* CreateStateT(Shibboleth::IApp& app)
{
	return Shibboleth::GetAllocator()->template allocT<State>(app);
}

enum States
{
	SETUP_GRAPHICS_STATE = 0,
	LOAD_COMPONENT_STATE,
	CREATE_RESOURCE_LOADERS_STATE,
	SETUP_OTTER_UI_STATE,
	LOOP_FOREVER_STATE,
	NUM_STATES
};

typedef Shibboleth::IState* (*CreateStateFunc)(Shibboleth::IApp&);

static CreateStateFunc create_funcs[] = {
	&CreateStateT<Shibboleth::SetupGraphicsState>,
	&CreateStateT<Shibboleth::LoadComponentsState>,
	&CreateStateT<Shibboleth::CreateResourceLoadersState>,
	&CreateStateT<Shibboleth::SetupOtterUIState>,
	&CreateStateT < LoopState >
};

static const char* state_names[NUM_STATES] = {
	"SetupGraphicsState",
	"LoadComponentState",
	"CreateResourceLoadersState",
	"SetupOtterUIState",
	"loopforeverstate"
};

static unsigned int g_image_alloc_tag = Gaff::FNV1Hash32("Images", static_cast<unsigned int>(strlen("Images")));

void* MEMCB ImageAlloc(const size_t size)
{
	return Shibboleth::GetAllocator()->alloc((unsigned int)size, g_image_alloc_tag);
}

void MEMCB ImageFree(const void* const data)
{
	Shibboleth::GetAllocator()->free((void*)data, g_image_alloc_tag);
}


DYNAMICEXPORT_C bool InitModule(Shibboleth::IApp& app)
{
	Gaff::JSON::SetMemoryFunctions(&Shibboleth::ShibbolethAllocate, &Shibboleth::ShibbolethFree);
	Gaff::JSON::SetHashSeed(app.getSeed());

	Shibboleth::CreateMemoryPool("Images", g_image_alloc_tag);
	Shibboleth::SetApp(app);

	Gaff::Image::SysInit();
	Gaff::Image::SetMemoryFunctions(ImageAlloc, ImageFree);

	return true;
}

DYNAMICEXPORT_C void ShutdownModule(void)
{
	Gaff::Image::SysShutdown();
}

DYNAMICEXPORT_C const char* GetStateName(unsigned int id)
{
	assert(id < NUM_STATES);
	return state_names[id];
}

DYNAMICEXPORT_C unsigned int GetNumStates(void)
{
	return NUM_STATES;
}

DYNAMICEXPORT_C Shibboleth::IState* CreateState(unsigned int id)
{
	assert(id < NUM_STATES);
	return create_funcs[id](Shibboleth::GetApp());
}

DYNAMICEXPORT_C void DestroyState(Shibboleth::IState* state, unsigned int)
{
	Shibboleth::GetAllocator()->freeT(state);
}
