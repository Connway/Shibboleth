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

#include <Shibboleth_BroadcasterManager.h>
#include <Shibboleth_ComponentManager.h>
#include <Shibboleth_OcclusionManager.h>
#include <Shibboleth_ResourceManager.h>
#include <Shibboleth_OtterUIManager.h>
#include <Shibboleth_RenderManager.h>
#include <Shibboleth_UpdateManager.h>
#include <Shibboleth_ObjectManager.h>
#include <Shibboleth_LuaManager.h>
#include <Shibboleth_Utilities.h>
#include <Shibboleth_IApp.h>
#include <Gaff_StackTrace.h>
#include <Gaff_JSON.h>

#ifdef USE_VLD
	#include <vld.h>
#endif

template <class Manager>
Shibboleth::IManager* CreateManagerT(Shibboleth::IApp& app)
{
	return Shibboleth::GetAllocator()->template allocT<Manager>(app);
}

template <class Manager>
Shibboleth::IManager* CreateManagerTNoApp(Shibboleth::IApp&)
{
	return Shibboleth::GetAllocator()->template allocT<Manager>();
}

Shibboleth::IManager* CreateOtterUIManager(Shibboleth::IApp& /*app*/)
{
	Shibboleth::OtterUIManager* otter_manager = Shibboleth::GetAllocator()->template allocT<Shibboleth::OtterUIManager>();

	if (otter_manager) {
		if (!otter_manager->init()) {
			Shibboleth::GetAllocator()->freeT(otter_manager);
			otter_manager = nullptr;
		}
	}

	return otter_manager;
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
	OCCLUSION_MANAGER,
	BROADCASTER_MANAGER,
	NUM_MANAGERS
};

typedef Shibboleth::IManager* (*CreateMgrFunc)(Shibboleth::IApp&);

static CreateMgrFunc create_funcs[] = {
	&CreateManagerT<Shibboleth::ComponentManager>,
	&CreateManagerT<Shibboleth::ResourceManager>,
	&CreateOtterUIManager,
	&CreateManagerT<Shibboleth::RenderManager>,
	&CreateManagerT<Shibboleth::UpdateManager>,
	&CreateManagerTNoApp<Shibboleth::ObjectManager>,
	&CreateManagerTNoApp<Shibboleth::LuaManager>,
	&CreateManagerTNoApp<Shibboleth::OcclusionManager>,
	&CreateManagerTNoApp<Shibboleth::BroadcasterManager>
};

DYNAMICEXPORT_C bool InitModule(Shibboleth::IApp& app)
{
	Gaff::JSON::SetMemoryFunctions(&Shibboleth::ShibbolethAllocate, &Shibboleth::ShibbolethFree);
	Gaff::JSON::SetHashSeed(app.getSeed());
	Shibboleth::SetApp(app);

	return true;
}

DYNAMICEXPORT_C void ShutdownModule(void)
{
}

DYNAMICEXPORT_C unsigned int GetNumManagers(void)
{
	return NUM_MANAGERS;
}

DYNAMICEXPORT_C Shibboleth::IManager* CreateManager(unsigned int id)
{
	assert(id < NUM_MANAGERS);
	return create_funcs[id](Shibboleth::GetApp());
}

DYNAMICEXPORT_C void DestroyManager(Shibboleth::IManager* manager, unsigned int)
{
	Shibboleth::GetAllocator()->freeT(manager);
}
