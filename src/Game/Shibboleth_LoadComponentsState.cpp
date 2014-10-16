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

#include <Shibboleth_LoadComponentsState.h>
#include <Shibboleth_ComponentManager.h>
#include <Shibboleth_String.h>
#include <Shibboleth_IApp.h>
#include <Gaff_Utils.h>

NS_SHIBBOLETH

LoadComponentsState::LoadComponentsState(IApp& app):
	_app(app)
{
}

LoadComponentsState::~LoadComponentsState(void)
{
}

bool LoadComponentsState::init(unsigned int /*state_id*/)
{
	// Make sure we have a transition for when we finish loading.
	// If we have more than one transition, you're using this state wrong
	// and should fix it.
	assert(_transitions.size() == 1);
	//_state_id = state_id;
	return true;
}

void LoadComponentsState::enter(void)
{
}

void LoadComponentsState::update(void)
{
	LogManager::FileLockPair& log = _app.getGameLogFile();
	//bool error = false;

	ComponentManager& comp_man = _app.getManagerT<ComponentManager>("Component Manager");

	log.first.writeString("==================================================\n");
	log.first.writeString("==================================================\n");
	log.first.writeString("Loading Components...\n");

	Gaff::ForEachTypeInDirectory<Gaff::FDT_RegularFile>("./Components", [&](const char* name, size_t) -> bool
	{
		AString rel_path = AString("./Components/") + name;

		// Error out if it's not a dynamic module
		if (!Gaff::File::checkExtension(name, DYNAMIC_EXTENSION)) {
			log.first.printf("ERROR - '%s' is not a dynamic module.\n", rel_path.getBuffer());
			//error = true;
			_app.quit();
			return true;

		// It is a dynamic module, but not compiled for our architecture.
		// Or not compiled in our build mode. Just skip over it.
		} else if (!Gaff::File::checkExtension(name, BIT_EXTENSION DYNAMIC_EXTENSION)) {
			return false;
		}

		DynamicLoader::ModulePtr module = _app.loadModule(rel_path.getBuffer(), name);

		if (!module.valid()) {
			log.first.printf("ERROR - Could not load dynamic module '%s'.\n", rel_path.getBuffer());
			//error = true;
			_app.quit();
			return true;
		}

		if (!comp_man.addComponents(module)) {
			log.first.printf("ERROR - Could not load components in dynamic module '%s'.\n", rel_path.getBuffer());
			//error = true;
			_app.quit();
			return true;
		}

		return false;
	});

	log.first.writeString("Finished Loading Components\n\n");
	_app.switchState(_transitions[0]);
}

void LoadComponentsState::exit(void)
{
}

NS_END
