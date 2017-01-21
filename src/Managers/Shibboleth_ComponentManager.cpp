/************************************************************************************
Copyright (C) 2017 by Nicholas LaCroix

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
#include "Shibboleth_Component.h"
#include <Shibboleth_Utilities.h>
#include <Shibboleth_IApp.h>
#include <Gaff_File.h>

NS_SHIBBOLETH

REF_IMPL_REQ(ComponentManager);

SHIB_REF_IMPL(ComponentManager)
.addBaseClassInterfaceOnly<ComponentManager>()
.ADD_BASE_CLASS_INTERFACE_ONLY(IComponentManager)
;

ComponentManager::ComponentManager(void)
{
}

ComponentManager::~ComponentManager(void)
{
}

const char* ComponentManager::getName(void) const
{
	return GetFriendlyName();
}

void ComponentManager::allManagersCreated(void)
{
	//bool error = false;

	GetApp().getLogManager().logMessage(
		LogManager::LOG_NORMAL, GetApp().getLogFileName(),
		"\n==================================================\n"
		"==================================================\n"
		"Loading Components...\n"
	);

	Gaff::ForEachTypeInDirectory<Gaff::FDT_RegularFile>("./Components", [&](const char* name, size_t) -> bool
	{
		U8String rel_path = U8String("./Components/") + name;

		// Error out if it's not a dynamic module
		if (!Gaff::File::CheckExtension(name, DYNAMIC_EXTENSION)) {
			GetApp().getLogManager().logMessage(LogManager::LOG_ERROR, GetApp().getLogFileName(), "ERROR - '%s' is not a dynamic module.\n", rel_path.getBuffer());
			//error = true;
			GetApp().quit();
			return true;

		// It is a dynamic module, but not compiled for our architecture.
		// Or not compiled in our build mode. Just skip over it.
		} else if (!Gaff::File::CheckExtension(name, BIT_EXTENSION DYNAMIC_EXTENSION)) {
			return false;
		}

#ifdef PLATFORM_WINDOWS
		DynamicLoader::ModulePtr module = GetApp().loadModule(("../" + rel_path).getBuffer(), name);
#else
		DynamicLoader::ModulePtr module = GetApp().loadModule(rel_path.getBuffer(), name);
#endif

		if (!module.valid()) {
			GetApp().getLogManager().logMessage(LogManager::LOG_ERROR, GetApp().getLogFileName(), "ERROR - Could not load dynamic module '%s'.\n", rel_path.getBuffer());
			//error = true;
			GetApp().quit();
			return true;
		}

		GetApp().getLogManager().logMessage(LogManager::LOG_NORMAL, GetApp().getLogFileName(), "Loading components from module '%s'\n", rel_path.getBuffer());

		if (!addComponents(module)) {
			GetApp().getLogManager().logMessage(LogManager::LOG_ERROR, GetApp().getLogFileName(), "ERROR - Could not load components in dynamic module '%s'.\n", rel_path.getBuffer());
			//error = true;
			GetApp().quit();
			return true;
		}

		return false;
	});

	GetApp().getLogManager().logMessage(LogManager::LOG_NORMAL, GetApp().getLogFileName(), "Finished Loading Components\n\n");
}

Component* ComponentManager::createComponent(const HashString32& name)
{
	size_t index = _components.indexOf(name);
	GAFF_ASSERT(name.size() && index != SIZE_T_FAIL);

	ComponentEntry& entry = _components.valueAt(index);
	Component* component = entry.create(entry.component_id);

	if (component) {
		component->setIndex(index);
	}

	return component;
}

Component* ComponentManager::createComponent(const char* name)
{
	GAFF_ASSERT(name && _components.indexOf(name) != SIZE_T_FAIL);
	return createComponent(HashString32(name));
}

void ComponentManager::destroyComponent(Component* component)
{
	GAFF_ASSERT(component);
	ComponentEntry& entry = _components.valueAt(component->getIndex());
	entry.destroy(component, entry.component_id);
}

bool ComponentManager::addComponents(DynamicLoader::ModulePtr& module)
{
	GAFF_ASSERT(module.valid());
	GetNumComponentsFunc num_comp_func = module->getFunc<GetNumComponentsFunc>("GetNumComponents");
	GetComponentNameFunc comp_name_func = module->getFunc<GetComponentNameFunc>("GetComponentName");
	CreateComponentFunc create_comp_func = module->getFunc<CreateComponentFunc>("CreateComponent");
	DestroyComponentFunc destroy_comp_func = module->getFunc<DestroyComponentFunc>("DestroyComponent");
	InitFunc init_func = module->getFunc<InitFunc>("InitModule");

	if (!init_func) {
		GetApp().getLogManager().logMessage(LogManager::LOG_ERROR, GetApp().getLogFileName(), "ERROR - Could not find function named 'InitModule'.\n");
		return false;
	}

	if (!num_comp_func) {
		GetApp().getLogManager().logMessage(LogManager::LOG_ERROR, GetApp().getLogFileName(), "ERROR - Could not find function named 'GetNumComponent'.\n");
		return false;
	}

	if (!comp_name_func) {
		GetApp().getLogManager().logMessage(LogManager::LOG_ERROR, GetApp().getLogFileName(), "ERROR - Could not find function named 'GetComponentName'.\n");
		return false;
	}

	if (!create_comp_func) {
		GetApp().getLogManager().logMessage(LogManager::LOG_ERROR, GetApp().getLogFileName(), "ERROR - Could not find function named 'CreateComponent'.\n");
		return false;
	}

	if (!destroy_comp_func) {
		GetApp().getLogManager().logMessage(LogManager::LOG_ERROR, GetApp().getLogFileName(), "ERROR - Could not find function named 'DestroyComponent'.\n");
		return false;
	}

	if (!init_func(GetApp())) {
		GetApp().getLogManager().logMessage(LogManager::LOG_ERROR, GetApp().getLogFileName(), "ERROR - Failed to initialize component module.\n");
		return false;
	}

	unsigned int num_comps = num_comp_func();

	for (unsigned int i = 0; i < num_comps; ++i) {
		HashString32 name = comp_name_func(i);

		if (_components.indexOf(name) != SIZE_T_FAIL) {
			GetApp().getLogManager().logMessage(LogManager::LOG_ERROR, GetApp().getLogFileName(), "ERROR - Component with name '%s' already registered.\n", name.getBuffer());
			return false;
		}

		ComponentEntry entry = {
			module,
			create_comp_func,
			destroy_comp_func,
			i
		};

		_components.insert(name, entry);
		GetApp().getLogManager().logMessage(LogManager::LOG_NORMAL, GetApp().getLogFileName(), "Loaded component '%s'\n", name.getBuffer());
	}

	return true;
}

NS_END
