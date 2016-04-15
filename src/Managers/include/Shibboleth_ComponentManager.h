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

#pragma once

#include "Shibboleth_Component.h"
#include <Shibboleth_ReflectionDefinitions.h>
#include <Shibboleth_DynamicLoader.h>
#include <Shibboleth_HashString.h>
#include <Shibboleth_IManager.h>
#include <Shibboleth_HashMap.h>
#include <Shibboleth_String.h>

NS_SHIBBOLETH

class IApp;

class ComponentManager : public IManager
{
public:
	static const char* GetFriendlyName(void);

	ComponentManager(void);
	~ComponentManager(void);

	const char* getName(void) const;

	void allManagersCreated(void);

	INLINE Component* createComponent(AHashString name);
	INLINE Component* createComponent(const char* name);
	INLINE void destroyComponent(Component* component);

private:
	using InitFunc = bool (*)(IApp&);
	using CreateComponentFunc = Component* (*)(unsigned int);
	using DestroyComponentFunc = void (*)(Component*, unsigned int);
	using GetNumComponentsFunc = unsigned int (*)(void);
	using GetComponentNameFunc = const char* (*)(unsigned int);

	struct ComponentEntry
	{
		DynamicLoader::ModulePtr module;
		CreateComponentFunc create;
		DestroyComponentFunc destroy;
		unsigned int component_id;
	};

	HashMap<AHashString, ComponentEntry> _components;

	bool addComponents(DynamicLoader::ModulePtr& module);

	GAFF_NO_COPY(ComponentManager);
	GAFF_NO_MOVE(ComponentManager);

	SHIB_REF_DEF(ComponentManager);
	REF_DEF_REQ;
};

NS_END
