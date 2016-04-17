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

// Managers
#include <Shibboleth_RenderPipelineManager.h>
#include <Shibboleth_BroadcasterManager.h>
#include <Shibboleth_ComponentManager.h>
#include <Shibboleth_OcclusionManager.h>
#include <Shibboleth_ResourceManager.h>
#include <Shibboleth_OtterUIManager.h>
#include <Shibboleth_CameraManager.h>
#include <Shibboleth_SchemaManager.h>
#include <Shibboleth_RenderManager.h>
#include <Shibboleth_UpdateManager.h>
#include <Shibboleth_ObjectManager.h>
#include <Shibboleth_InputManager.h>
#include <Shibboleth_FrameManager.h>
#include <Shibboleth_LuaManager.h>

#ifndef USE_PHYSX
	#include <Shibboleth_BulletPhysicsManager.h>
#else
	#include <Shibboleth_PhysXPhysicsManager.h>
#endif

// Other Dependencies
#include <Shibboleth_ModuleHelpers.h>
#include <Shibboleth_Utilities.h>
#include <Shibboleth_IApp.h>

static Shibboleth::CreateManagerFunc create_funcs[] = {
	Shibboleth::CreateManagerT<Shibboleth::ComponentManager>,
	Shibboleth::CreateManagerT<Shibboleth::ResourceManager>,
	Shibboleth::CreateManagerWithInitT<Shibboleth::OtterUIManager>,
	Shibboleth::CreateManagerT<Shibboleth::RenderManager>,
	Shibboleth::CreateManagerT<Shibboleth::UpdateManager>,
	Shibboleth::CreateManagerT<Shibboleth::ObjectManager>,
	Shibboleth::CreateManagerT<Shibboleth::LuaManager>,
	Shibboleth::CreateManagerT<Shibboleth::OcclusionManager>,
	Shibboleth::CreateManagerT<Shibboleth::BroadcasterManager>,
	Shibboleth::CreateManagerT<Shibboleth::CameraManager>,
	Shibboleth::CreateManagerWithInitT<Shibboleth::FrameManager>,
	Shibboleth::CreateManagerT<Shibboleth::RenderPipelineManager>,
	Shibboleth::CreateManagerT<Shibboleth::SchemaManager>,
#ifndef USE_PHYSX
	Shibboleth::CreateManagerT<Shibboleth::BulletPhysicsManager>,
#else
	Shibboleth::CreateManagerT<Shibboleth::PhysXPhysicsManager>,
#endif
	Shibboleth::CreateManagerT<Shibboleth::InputManager>
};

DYNAMICEXPORT_C bool InitModule(Shibboleth::IApp& app)
{
	Shibboleth::SetApp(app);
	return true;
}

DYNAMICEXPORT_C void ShutdownModule(void)
{
}

DYNAMICEXPORT_C unsigned int GetNumManagers(void)
{
	return ARRAY_SIZE(create_funcs);
}

DYNAMICEXPORT_C Shibboleth::IManager* CreateManager(unsigned int id)
{
	GAFF_ASSERT(id < ARRAY_SIZE(create_funcs));
	return create_funcs[id]();
}

DYNAMICEXPORT_C void DestroyManager(Shibboleth::IManager* manager, unsigned int)
{
	SHIB_FREET(manager, *Shibboleth::GetAllocator());
}
