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

#include <Shibboleth_ComponentManager.h>
#include <Shibboleth_CameraComponent.h>
#include <Shibboleth_ModelComponent.h>
#include <Shibboleth_LuaComponent.h>

#include <Shibboleth_ModuleHelpers.h>
#include <Shibboleth_Utilities.h>
#include <Shibboleth_IApp.h>
#include <Gaff_JSON.h>

#ifdef USE_PHYSX
	#include <Shibboleth_PhysXPhysicsComponent.h>
	#include <Shibboleth_PhysXPhysicsManager.h>
#else
	#include <Shibboleth_BulletPhysicsComponent.h>
	#include <Shibboleth_BulletPhysicsManager.h>
#endif

static Shibboleth::ComponentFuncs components[] = {
	{ Shibboleth::CreateComponentT<Shibboleth::LuaComponent>, Shibboleth::LuaComponent::GetFriendlyName },
	{ Shibboleth::CreateComponentT<Shibboleth::ModelComponent>, Shibboleth::ModelComponent::GetFriendlyName },
	{ Shibboleth::CreateComponentT<Shibboleth::CameraComponent>, Shibboleth::CameraComponent::GetFriendlyName },
#ifdef USE_PHYSX
	{ Shibboleth::CreateComponentT<Shibboleth::PhysXPhysicsComponent>, Shibboleth::PhysXPhysicsComponent::GetFriendlyName },
#else
	{ Shibboleth::CreateComponentT<Shibboleth::BulletPhysicsComponent>, Shibboleth::BulletPhysicsComponent::GetFriendlyName },
#endif
};

DYNAMICEXPORT_C bool InitModule(Shibboleth::IApp& app)
{
	Gaff::JSON::SetHashSeed(app.getSeed());
	Shibboleth::SetApp(app);

	return true;
}

DYNAMICEXPORT_C void ShutdownModule(void)
{
}

DYNAMICEXPORT_C const char* GetComponentName(unsigned int id)
{
	GAFF_ASSERT(id < ARRAY_SIZE(components));
	return components[id].name();
}

DYNAMICEXPORT_C unsigned int GetNumComponents(void)
{
	return ARRAY_SIZE(components);
}

DYNAMICEXPORT_C Shibboleth::Component* CreateComponent(unsigned int id)
{
	GAFF_ASSERT(id < ARRAY_SIZE(components));
	return components[id].create();
}

DYNAMICEXPORT_C void DestroyComponent(Shibboleth::Component* component, unsigned int)
{
	SHIB_FREET(component, *Shibboleth::GetAllocator());
}
