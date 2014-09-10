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

#include <Shibboleth_ComponentManager.h>
#include <Shibboleth_ResourceManager.h>
#include <Shibboleth_OtterUIManager.h>
#include <Shibboleth_RenderManager.h>
#include <Shibboleth_UpdateManager.h>
#include <Shibboleth_ObjectManager.h>
#include <Shibboleth_LuaManager.h>
#include <Gaff_JSON.h>

template <class Manager>
Shibboleth::IManager* CreateManagerT(Shibboleth::App& app)
{
	return app.getAllocator().template allocT<Manager>(app);
}

template <class Manager>
Shibboleth::IManager* CreateManagerTNoApp(Shibboleth::App& app)
{
	return app.getAllocator().template allocT<Manager>();
}

Shibboleth::IManager* CreateOtterUIManager(Shibboleth::App& app)
{
	Shibboleth::OtterUIManager* otter_manager = app.getAllocator().template allocT<Shibboleth::OtterUIManager>();

	if (otter_manager) {
		if (!otter_manager->init()) {
			app.getAllocator().freeT(otter_manager);
			otter_manager = nullptr;
		}
	}

	return otter_manager;
}

Shibboleth::IManager* CreateRenderManager(Shibboleth::App& app)
{
	Shibboleth::RenderManager* render_manager = app.getAllocator().template allocT<Shibboleth::RenderManager>(app);

	if (render_manager) {
		if (!render_manager->init("graphics.cfg")) {
			app.getAllocator().freeT(render_manager);
			render_manager = nullptr;
		}
	}

	return render_manager;
}

enum Managers
{
	COMPONENT_MANAGER = 0,
	RESOURCE_MANAGER,
	OTTERUI_MANAGER,
	RENDER_MANAGER,
	UPDATE_MANAGER,
	OBJECT_MANAGER,
	LUA_MANAGER,
	NUM_MANAGERS
};

typedef Shibboleth::IManager* (*CreateMgrFunc)(Shibboleth::App&);

static CreateMgrFunc create_funcs[] = {
	&CreateManagerT<Shibboleth::ComponentManager>,
	&CreateManagerT<Shibboleth::ResourceManager>,
	&CreateOtterUIManager,
	&CreateRenderManager,
	&CreateManagerT<Shibboleth::UpdateManager>,
	&CreateManagerT<Shibboleth::ObjectManager>,
	&CreateManagerT<Shibboleth::LuaManager>,
};

static Shibboleth::App* g_app = nullptr;

DYNAMICEXPORT bool InitModule(Shibboleth::App& app)
{
	Gaff::JSON::SetMemoryFunctions(&Shibboleth::ShibbolethAllocate, &Shibboleth::ShibbolethFree);
	Gaff::JSON::SetHashSeed(0);
	Shibboleth::SetAllocator(app.getAllocator());
	g_app = &app;
	return true;
}

DYNAMICEXPORT unsigned int GetNumManagers(void)
{
	return NUM_MANAGERS;
}

DYNAMICEXPORT Shibboleth::IManager* CreateManager(unsigned int id)
{
	assert(id < NUM_MANAGERS);
	return create_funcs[id](*g_app);
}

DYNAMICEXPORT void DestroyManager(Shibboleth::IManager* manager, unsigned int)
{
	Shibboleth::GetAllocator()->freeT(manager);
}
