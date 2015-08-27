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

#include "Shibboleth_ComponentManager.h"
#include <Shibboleth_Utilities.h>
#include <Shibboleth_IApp.h>

NS_SHIBBOLETH

REF_IMPL_REQ(ComponentManager);

SHIB_REF_IMPL(ComponentManager)
.addBaseClassInterfaceOnly<ComponentManager>()
;

ComponentManager::ComponentManager(void)
{
}

ComponentManager::~ComponentManager(void)
{
}

const char* ComponentManager::getName(void) const
{
	return "Component Manager";
}

void ComponentManager::allManagersCreated(void)
{
	LogManager::FileLockPair& log = GetApp().getGameLogFile();
	//bool error = false;

	log.first.writeString("\n==================================================\n");
	log.first.writeString("==================================================\n");
	log.first.writeString("Loading Components...\n");

	Gaff::ForEachTypeInDirectory<Gaff::FDT_RegularFile>("./Components", [&](const char* name, size_t) -> bool
	{
		AString rel_path = AString("./Components/") + name;

		// Error out if it's not a dynamic module
		if (!Gaff::File::checkExtension(name, DYNAMIC_EXTENSION)) {
			log.first.printf("ERROR - '%s' is not a dynamic module.\n", rel_path.getBuffer());
			//error = true;
			GetApp().quit();
			return true;

		// It is a dynamic module, but not compiled for our architecture.
		// Or not compiled in our build mode. Just skip over it.
		} else if (!Gaff::File::checkExtension(name, BIT_EXTENSION DYNAMIC_EXTENSION)) {
			return false;
		}

		DynamicLoader::ModulePtr module = GetApp().loadModule(rel_path.getBuffer(), name);

		if (!module.valid()) {
			log.first.printf("ERROR - Could not load dynamic module '%s'.\n", rel_path.getBuffer());
			//error = true;
			GetApp().quit();
			return true;
		}

		log.first.printf("Loading components from module '%s'\n", rel_path.getBuffer());

		if (!addComponents(module)) {
			log.first.printf("ERROR - Could not load components in dynamic module '%s'.\n", rel_path.getBuffer());
			//error = true;
			GetApp().quit();
			return true;
		}

		return false;
	});

	log.first.writeString("Finished Loading Components\n\n");
}

Component* ComponentManager::createComponent(AHashString name)
{
	size_t index = _components.indexOf(name);
	assert(name.size() && index != SIZE_T_FAIL);

	ComponentEntry& entry = _components.valueAt(index);
	Component* component = entry.create(entry.component_id);

	if (component) {
		component->setIndex(index);
	}

	return component;
}

Component* ComponentManager::createComponent(const char* name)
{
	assert(name && _components.indexOf(name) != SIZE_T_FAIL);
	return createComponent(AHashString(name));
}

void ComponentManager::destroyComponent(Component* component)
{
	assert(component);
	ComponentEntry& entry = _components.valueAt(component->getIndex());
	entry.destroy(component, entry.component_id);
}

bool ComponentManager::addComponents(DynamicLoader::ModulePtr& module)
{
	LogManager::FileLockPair& log = GetApp().getGameLogFile();

	assert(module.valid());
	GetNumComponentsFunc num_comp_func = module->getFunc<GetNumComponentsFunc>("GetNumComponents");
	GetComponentNameFunc comp_name_func = module->getFunc<GetComponentNameFunc>("GetComponentName");
	CreateComponentFunc create_comp_func = module->getFunc<CreateComponentFunc>("CreateComponent");
	DestroyComponentFunc destroy_comp_func = module->getFunc<DestroyComponentFunc>("DestroyComponent");
	InitFunc init_func = module->getFunc<InitFunc>("InitModule");

	if (!init_func) {
		log.first.writeString("ERROR - Could not find function named 'InitModule'.\n");
		return false;
	}

	if (!num_comp_func) {
		log.first.writeString("ERROR - Could not find function named 'GetNumComponent'.\n");
		return false;
	}

	if (!comp_name_func) {
		log.first.writeString("ERROR - Could not find function named 'GetComponentName'.\n");
		return false;
	}

	if (!create_comp_func) {
		log.first.writeString("ERROR - Could not find function named 'CreateComponent'.\n");
		return false;
	}

	if (!destroy_comp_func) {
		log.first.writeString("ERROR - Could not find function named 'DestroyComponent'.\n");
		return false;
	}

	if (!init_func(GetApp())) {
		log.first.writeString("ERROR - Failed to initialize component module.\n");
		return false;
	}

	unsigned int num_comps = num_comp_func();

	for (unsigned int i = 0; i < num_comps; ++i) {
		AHashString name = comp_name_func(i);

		if (_components.indexOf(name) != SIZE_T_FAIL) {
			log.first.printf("ERROR - Component with name '%s' already registered.\n", name.getBuffer());
			return false;
		}

		ComponentEntry entry = {
			module,
			create_comp_func,
			destroy_comp_func,
			i
		};

		_components.insert(name, entry);
		log.first.printf("Loaded component '%s'\n", name.getBuffer());
	}

	return true;
}

NS_END
