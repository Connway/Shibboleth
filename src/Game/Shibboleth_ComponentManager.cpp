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

#include "Shibboleth_ComponentManager.h"

NS_SHIBBOLETH

ComponentManager::ComponentManager(App& app):
	_app(app)
{
}

ComponentManager::~ComponentManager(void)
{
}

const char* ComponentManager::getName(void) const
{
	return "Component Manager";
}

bool ComponentManager::addComponents(DynamicLoader::ModulePtr& module)
{
	Gaff::File& log = _app.getGameLogFile();

	assert(module.valid());
	GetNumComponentsFunc num_comp_func = module->GetFunc<GetNumComponentsFunc>("GetNumComponents");
	GetComponentNameFunc comp_name_func = module->GetFunc<GetComponentNameFunc>("GetComponentName");
	CreateComponentFunc create_comp_func = module->GetFunc<CreateComponentFunc>("CreateComponent");
	DestroyComponentFunc destroy_comp_func = module->GetFunc<DestroyComponentFunc>("DestroyComponent");

	if (!num_comp_func) {
		log.writeString("ERROR - Could not find function named 'GetNumComponent'.\n");
		return false;
	}

	if (!comp_name_func) {
		log.writeString("ERROR - Could not find function named 'GetComponentName'.\n");
		return false;
	}

	if (!create_comp_func) {
		log.writeString("ERROR - Could not find function named 'CreateComponent'.\n");
		return false;
	}

	if (!destroy_comp_func) {
		log.writeString("ERROR - Could not find function named 'DestroyComponent'.\n");
		return false;
	}

	unsigned int num_comps = num_comp_func();

	for (unsigned int i = 0; i < num_comps; ++i) {
		AHashString name = comp_name_func(i);

		if (_components.indexOf(name) != -1) {
			log.printf("ERROR - Component with name '%s' already registered.\n", name.getBuffer());
			return false;
		}

		ComponentEntry entry = {
			module,
			create_comp_func,
			destroy_comp_func,
			i
		};

		_components.insert(name, entry);
		log.printf("'%s' loaded\n", name.getBuffer());
	}

	return true;
}

IComponent* ComponentManager::createComponent(AHashString name)
{
	assert(name.size() && _components.indexOf(name) != -1);
	ComponentEntry& entry = _components[name];
	ProxyAllocator allocator;
	return entry.create(allocator, _app, entry.component_id);
}

IComponent* ComponentManager::createComponent(AString name)
{
	assert(name.size() && _components.indexOf(name) != -1);
	return createComponent(AHashString(name));
}

IComponent* ComponentManager::createComponent(const char* name)
{
	assert(name && _components.indexOf(name) != -1);
	return createComponent(AHashString(name));
}

void ComponentManager::destroyComponent(IComponent* component)
{
	assert(component && _components.indexOf(component->getName()) != -1);
	ComponentEntry& entry = _components[component->getName()];
	ProxyAllocator allocator;
	entry.destroy(allocator, component, entry.component_id);
}

NS_END