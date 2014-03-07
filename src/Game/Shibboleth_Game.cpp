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

#include "Shibboleth_Game.h"
#include "Shibboleth_String.h"
#include <Gaff_Utils.h>
#include <Gaff_File.h>
#include <Gaff_JSON.h>
#include <iostream>

NS_SHIBBOLETH

Game::Game(void):
	_running(true)
{
}

Game::~Game(void)
{
	for (ManagerMap::Iterator it = _manager_map.begin(); it != _manager_map.end(); ++it) {
		it->destroy_func(GetAllocator(), it->manager);
	}

	//_state_machine.clear();

	_dynamic_loader.clear();
}

bool Game::init(void)
{
	Gaff::JSON::SetMemoryFunctions(&ShibbolethAllocate, &ShibbolethFree);

	loadManagers();

	if (!loadStates()) {
		return false;
	}

	return true;
}

void Game::loadManagers(void)
{
	// Load managers from DLLs
	Gaff::ForEachTypeInDirectory<Gaff::DT_RegularFile>("./Managers", [&](const char* name, size_t name_len) -> bool
	{
		if (!Gaff::File::checkExtension(name, DYNAMIC_EXTENSION)) {
			return false;
		}

		AString rel_path = AString("./Managers/") + name;

		DynamicLoader::ModulePtr module = _dynamic_loader.loadModule(rel_path.getBuffer(), name);

		if (module) {
			ManagerEntry entry = { nullptr, nullptr, nullptr };
			entry.create_func = module->GetFunc<ManagerEntry::CreateManagerFunc>("CreateManager");
			entry.destroy_func = module->GetFunc<ManagerEntry::DestroyManagerFunc>("DestroyManager");

			if (entry.create_func && entry.destroy_func) {
				entry.manager = entry.create_func(GetAllocator());

				if (entry.manager) {
					_manager_map[entry.manager->GetName()] = entry;
				}
			} else {
				_dynamic_loader.removeModule(name);
			}
		}

		return false;
	});
}

bool Game::loadStates(void)
{
	Gaff::JSON state_data;

	if (!state_data.parseFile("./States/states.json")) {
		return false;
	}

	if (!state_data.isObject()) {
		return false;
	}

	Gaff::JSON starting_state = state_data["starting_state"];
	Gaff::JSON states = state_data["states"];

	if (!starting_state.isString() || !states.isArray()) {
		return false;
	}

	for (size_t i = 0; i < states.size(); ++i) {
		Gaff::JSON state = states[i];

		if (!state.isObject()) {
			return false;
		}

		Gaff::JSON transitions = state["transitions"];
		Gaff::JSON name = state["name"];

		if (!transitions.isArray() || !name.isString()) {
			return false;
		}

		AString filename("./States/");
		filename += name.getString();
		filename += DYNAMIC_EXTENSION;

		DynamicLoader::ModulePtr module = _dynamic_loader.loadModule(filename, name.getString());

		if (module) {
			StateMachine::StateEntry entry = { Array<unsigned int>(), name.getString(), nullptr, nullptr, nullptr };
			entry.create_func = module->GetFunc<StateMachine::StateEntry::CreateStateFunc>("CreateState");
			entry.destroy_func = module->GetFunc<StateMachine::StateEntry::DestroyStateFunc>("DestroyState");

			if (entry.create_func && entry.destroy_func) {
				entry.state = entry.create_func(GetAllocator());

				if (entry.state) {
					entry.transitions.reserve(transitions.size());

					for (size_t j = 0; j < transitions.size(); ++j) {
						Gaff::JSON val = transitions[j];

						if (!val.isInteger()) {
							return false;
						}

						entry.transitions.push((unsigned int)val.getInteger());
					}

					//_state_machine.addState(entry);
				}
			} else {
				_dynamic_loader.removeModule(name.getString());
			}
		}
	}

	return true;
}

void Game::run(void)
{
	while (_running) {
	}
}

NS_END
