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


template <class Component>
Shibboleth::Component* CreateComponent(void)
{
	return SHIB_ALLOCT(Component, *Shibboleth::GetAllocator());
}

enum Components
{
	LUA_COMPONENT = 0,
	MODEL_COMPONENT,
	CAMERA_COMPONENT,
	PHYSICS_COMPONENT,
	NUM_COMPONENTS
};

typedef Shibboleth::Component* (*CreateComponentFunc)(void);
typedef const char* (*ComponentNameFunc)(void);
typedef void (*RefDefClearFunc)(void);

static CreateComponentFunc create_funcs[] = {
	&CreateComponent<Shibboleth::LuaComponent>,
	&CreateComponent<Shibboleth::ModelComponent>,
	&CreateComponent<Shibboleth::CameraComponent>,
#ifndef USE_PHYSX
	&CreateComponent<Shibboleth::BulletPhysicsComponent>
#else
	&CreateComponent<Shibboleth::PhysXPhysicsComponent>
#endif
};

static ComponentNameFunc name_funcs[] = {
	&Shibboleth::LuaComponent::getComponentName,
	&Shibboleth::ModelComponent::getComponentName,
	&Shibboleth::CameraComponent::getComponentName,
#ifdef USE_PHYSX
	&Shibboleth::PhysXPhysicsComponent::getComponentName,
#else
	&Shibboleth::BulletPhysicsComponent::getComponentName,
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
	GAFF_ASSERT(id < NUM_COMPONENTS);
	return name_funcs[id]();
}

DYNAMICEXPORT_C unsigned int GetNumComponents(void)
{
	return NUM_COMPONENTS;
}

DYNAMICEXPORT_C Shibboleth::Component* CreateComponent(unsigned int id)
{
	GAFF_ASSERT(id < NUM_COMPONENTS);
	return create_funcs[id]();
}

DYNAMICEXPORT_C void DestroyComponent(Shibboleth::Component* component, unsigned int)
{
	SHIB_FREET(component, *Shibboleth::GetAllocator());
}
