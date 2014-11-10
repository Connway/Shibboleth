/************************************************************************************
Copyright (C) 2014 by Nicholas LaCroix

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

#include "Shibboleth_CreateResourceLoadersState.h"
#include "Shibboleth_LoadComponentsState.h"
#include "Shibboleth_SetupOtterUIState.h"
#include <Shibboleth_IApp.h>
#include <Gleam_Global.h>
#include <Gaff_JSON.h>

#include <Shibboleth_ModelComponent.h>
#include <Shibboleth_ObjectManager.h>
#include <Shibboleth_RenderManager.h>
#include <Shibboleth_Object.h>
#include <Gleam_IRenderDevice.h>

class LoopState : public Shibboleth::IState
{
public:
	LoopState(Shibboleth::IApp& app): _object(nullptr), _app(app) {}

	bool init(unsigned int)
	{
		return true;
	}

	void enter(void)
	{
		_object = _app.getManagerT<Shibboleth::ObjectManager>("Object Manager").createObject();
		
		if (_object) {
			_object->init("Resources/Objects/test.object");
		}
	}

	void update(void)
	{
		Shibboleth::RenderManager& rm = _app.getManagerT<Shibboleth::RenderManager>("Render Manager");
		rm.updateWindows();

		Shibboleth::ModelComponent* model = _object->getFirstComponentWithInterface<Shibboleth::ModelComponent>();

		rm.getRenderDevice().beginFrame();

		if (model) {
			model->render();
		}

		rm.getRenderDevice().endFrame();
	}

	void exit(void)
	{
	}

private:
	Shibboleth::Object* _object;
	Shibboleth::IApp& _app;
};

template <class State>
Shibboleth::IState* CreateStateT(Shibboleth::IApp& app)
{
	return app.getAllocator().template allocT<State>(app);
}

enum States
{
	LOAD_COMPONENT_STATE = 0,
	CREATE_RESOURCE_LOADERS_STATE,
	SETUP_OTTER_UI_STATE,
	LOOP_FOREVER_STATE,
	NUM_STATES
};

typedef Shibboleth::IState* (*CreateStateFunc)(Shibboleth::IApp&);

static CreateStateFunc create_funcs[] = {
	&CreateStateT<Shibboleth::LoadComponentsState>,
	&CreateStateT<Shibboleth::CreateResourceLoadersState>,
	&CreateStateT<Shibboleth::SetupOtterUIState>,
	&CreateStateT<LoopState>
};

static const char* state_names[NUM_STATES] = {
	"LoadComponentState",
	"CreateResourceLoadersState",
	"SetupOtterUIState",
	"loopforeverstate"
};

static Shibboleth::IApp* g_app = nullptr;

DYNAMICEXPORT_C bool InitModule(Shibboleth::IApp& app)
{
	Gaff::JSON::SetMemoryFunctions(&Shibboleth::ShibbolethAllocate, &Shibboleth::ShibbolethFree);
	Gaff::JSON::SetHashSeed(app.getSeed());
	g_app = &app;
	return true;
}

DYNAMICEXPORT_C void ShutdownModule(void)
{
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
	return create_funcs[id](*g_app);
}

DYNAMICEXPORT_C void DestroyState(Shibboleth::IState* state, unsigned int)
{
	Shibboleth::GetAllocator()->freeT(state);
}
