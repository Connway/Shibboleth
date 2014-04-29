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

template <class Manager>
Gaff::INamedObject* CreateManagerT(Shibboleth::App& app)
{
	return app.getAllocator().template allocT<Manager>(app);
}

enum Managers
{
	COMPONENT_MANAGER = 0,
	RESOURCE_MANAGER,
	NUM_MANAGERS
};


typedef Gaff::INamedObject* (*CreateMgrFunc)(Shibboleth::App&);

CreateMgrFunc create_funcs[] = {
	&CreateManagerT<Shibboleth::ComponentManager>,
	&CreateManagerT<Shibboleth::ResourceManager>
};

DYNAMICEXPORT unsigned int GetNumManagers(void)
{
	return NUM_MANAGERS;
}

DYNAMICEXPORT Gaff::INamedObject* CreateManager(Shibboleth::App& app, unsigned int id)
{
	assert(id < NUM_MANAGERS);
	Shibboleth::SetAllocator(app.getAllocator());
	return create_funcs[id](app);
}

DYNAMICEXPORT void DestroyManager(Gaff::INamedObject* manager, unsigned int)
{
	Shibboleth::GetAllocator().freeT(manager);
}
