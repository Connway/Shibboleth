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

#pragma once

#include "Shibboleth_App.h"
#include "Shibboleth_IComponent.h"
#include "Shibboleth_HashString.h"
#include "Shibboleth_HashMap.h"

NS_SHIBBOLETH

class ComponentManager : public Gaff::INamedObject
{
public:
	ComponentManager(App& app);
	~ComponentManager(void);

	const char* getName(void) const;

	bool addComponents(DynamicLoader::ModulePtr& module);

	INLINE IComponent* createComponent(AHashString name);
	INLINE IComponent* createComponent(AString name);
	INLINE IComponent* createComponent(const char* name);
	INLINE void destroyComponent(IComponent* component);

private:
	typedef IComponent* (*CreateComponentFunc)(ProxyAllocator& allocator, App& app, unsigned int id);
	typedef void (*DestroyComponentFunc)(ProxyAllocator& allocator, IComponent* component, unsigned int id);
	typedef unsigned int (*GetNumComponentsFunc)(void);
	typedef const char* (*GetComponentNameFunc)(unsigned int);

	struct ComponentEntry
	{
		DynamicLoader::ModulePtr module;
		CreateComponentFunc create;
		DestroyComponentFunc destroy;
		unsigned int component_id;
	};

	HashMap<AHashString, ComponentEntry> _components;
	App& _app;
};

NS_END
