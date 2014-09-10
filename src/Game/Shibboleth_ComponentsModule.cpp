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
#include <Shibboleth_ModelComponent.h>
#include <Shibboleth_LuaComponent.h>
#include <Shibboleth_App.h>
#include <Gaff_JSON.h>

template <class Component>
Shibboleth::IComponent* CreateComponent(Shibboleth::App& app)
{
	return Shibboleth::GetAllocator()->allocT<Component>(app);
}

enum Components
{
	LUA_COMPONENT = 0,
	MODEL_COMPONENT,
	NUM_COMPONENTS
};

typedef Shibboleth::IComponent* (*CreateComponentFunc)(Shibboleth::App&);
typedef const char* (*ComponentNameFunc)(void);
typedef void (*RefDefInitFunc)(void);

static CreateComponentFunc create_funcs[] = {
	&CreateComponent<Shibboleth::LuaComponent>,
	&CreateComponent<Shibboleth::ModelComponent>
};

static ComponentNameFunc name_funcs[] = {
	&Shibboleth::LuaComponent::getComponentName,
	&Shibboleth::ModelComponent::getComponentName
};

static RefDefInitFunc ref_def_init_funcs[] = {
	&Shibboleth::LuaComponent::InitReflectionDefinition,
	&Shibboleth::ModelComponent::InitReflectionDefinition
};

static Shibboleth::App* g_app = nullptr;


DYNAMICEXPORT bool InitModule(Shibboleth::App& app)
{
	// set lua allocation function

	Gaff::JSON::SetMemoryFunctions(&Shibboleth::ShibbolethAllocate, &Shibboleth::ShibbolethFree);
	Gaff::JSON::SetHashSeed(app.getSeed());
	Shibboleth::SetAllocator(app.getAllocator());
	g_app = &app;

	// Initialize all the reflection definitions
	for (unsigned int i = 0; i < NUM_COMPONENTS; ++i) {
		ref_def_init_funcs[i]();
	}

	return true;
}

DYNAMICEXPORT const char* GetComponentName(unsigned int id)
{
	assert(id < NUM_COMPONENTS);
	return name_funcs[id]();
}

DYNAMICEXPORT unsigned int GetNumComponents(void)
{
	return NUM_COMPONENTS;
}

DYNAMICEXPORT Shibboleth::IComponent* CreateComponent(unsigned int id)
{
	assert(id < NUM_COMPONENTS);
	return create_funcs[id](*g_app);
}

DYNAMICEXPORT void DestroyComponent(Shibboleth::IComponent* component, unsigned int)
{
	Shibboleth::GetAllocator()->freeT(component);
}
