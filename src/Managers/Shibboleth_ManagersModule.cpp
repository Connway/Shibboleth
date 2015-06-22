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

// Managers
#include <Shibboleth_RenderPipelineManager.h>
#include <Shibboleth_BroadcasterManager.h>
#include <Shibboleth_ComponentManager.h>
#include <Shibboleth_OcclusionManager.h>
#include <Shibboleth_ResourceManager.h>
#include <Shibboleth_OtterUIManager.h>
#include <Shibboleth_CameraManager.h>
#include <Shibboleth_RenderManager.h>
#include <Shibboleth_UpdateManager.h>
#include <Shibboleth_ObjectManager.h>
#include <Shibboleth_FrameManager.h>
#include <Shibboleth_LuaManager.h>

// Other Dependencies
#include <Shibboleth_Utilities.h>
#include <Shibboleth_IApp.h>

#ifdef USE_VLD
	#include <vld.h>
#endif

template <class Manager>
Shibboleth::IManager* CreateManagerT(void)
{
	return Shibboleth::GetAllocator()->template allocT<Manager>();
}

template <class Manager>
Shibboleth::IManager* CreateManagerWithInitT(void)
{
	Manager* manager = Shibboleth::GetAllocator()->template allocT<Manager>();

	if (manager) {
		if (!manager->init()) {
			Shibboleth::GetAllocator()->freeT(manager);
			manager = nullptr;
		}
	}

	return manager;
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
	CAMERA_MANAGER,
	FRAME_MANAGER,
	RP_MANAGER,
	NUM_MANAGERS
};

using CreateMgrFunc = Shibboleth::IManager* (*)(void);

static CreateMgrFunc create_funcs[] = {
	&CreateManagerT<Shibboleth::ComponentManager>,
	&CreateManagerT<Shibboleth::ResourceManager>,
	&CreateManagerWithInitT<Shibboleth::OtterUIManager>,
	&CreateManagerT<Shibboleth::RenderManager>,
	&CreateManagerT<Shibboleth::UpdateManager>,
	&CreateManagerT<Shibboleth::ObjectManager>,
	&CreateManagerT<Shibboleth::LuaManager>,
	&CreateManagerT<Shibboleth::OcclusionManager>,
	&CreateManagerT<Shibboleth::BroadcasterManager>,
	&CreateManagerT<Shibboleth::CameraManager>,
	&CreateManagerWithInitT<Shibboleth::FrameManager>,
	&CreateManagerT<Shibboleth::RenderPipelineManager>,
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
	return create_funcs[id]();
}

DYNAMICEXPORT_C void DestroyManager(Shibboleth::IManager* manager, unsigned int)
{
	Shibboleth::GetAllocator()->freeT(manager);
}
