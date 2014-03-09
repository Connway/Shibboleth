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
#include "Shibboleth_Allocator.h"
#include "Shibboleth_String.h"
#include <Gaff_Utils.h>
#include <Gaff_File.h>
#include <Gaff_JSON.h>
#include <iostream>

NS_SHIBBOLETH

Game::Game(void):
	_thread_pool(ProxyAllocator()), _allocator(GetAllocator()),
	_logger(GetLogger()), _running(true)
{
}

Game::~Game(void)
{
	for (ManagerMap::Iterator it = _manager_map.begin(); it != _manager_map.end(); ++it) {
		it->destroy_func(ProxyAllocator(), it->manager);
	}

	_state_machine.clear();
	_dynamic_loader.clear();
}

bool Game::init(void)
{
	char logFilename[64] = { 0 };
	Gaff::GetCurrentTimeString(logFilename, 64, "Logs/GameLog %m-%d-%Y %H-%M.txt");

	if (!Gaff::CreateDir("./Logs", 0777) || !_logger.openLogFile(logFilename)) {
		return false;
	}

	_log_file = &_logger.getLogFile(logFilename);

	_log_file->writeString("==================================================\n");
	_log_file->writeString("==================================================\n");
	_log_file->writeString("Initializing Game\n");

	Gaff::JSON::SetMemoryFunctions(&ShibbolethAllocate, &ShibbolethFree);

	if (!_thread_pool.init(128)) {
		_log_file->writeString("ERROR - Failed to initialize thread pool\n");
		return false;
	}

	if (!loadManagers()) {
		return false;
	}

	if (!loadStates()) {
		return false;
	}

	_log_file->writeString("Game Successfully Initialized\n");
	return true;
}

bool Game::loadManagers(void)
{
	_log_file->writeString("Loading Managers\n");

	bool error = false;

	// Load managers from DLLs
	Gaff::ForEachTypeInDirectory<Gaff::FDT_RegularFile>("./Managers", [&](const char* name, size_t name_len) -> bool
	{
		AString rel_path = AString("./Managers/") + name;

		if (!Gaff::File::checkExtension(name, DYNAMIC_EXTENSION)) {
			_log_file->printf("ERROR - '%s' is not a dynamic module.\n", rel_path.getBuffer());
			error = true;
			return true;
		}

		DynamicLoader::ModulePtr module = _dynamic_loader.loadModule(rel_path.getBuffer(), name);

		if (module) {
			ManagerEntry entry = { nullptr, nullptr, nullptr };
			entry.create_func = module->GetFunc<ManagerEntry::CreateManagerFunc>("CreateManager");
			entry.destroy_func = module->GetFunc<ManagerEntry::DestroyManagerFunc>("DestroyManager");

			if (entry.create_func && entry.destroy_func) {
				entry.manager = entry.create_func(ProxyAllocator(), *this);

				if (entry.manager) {
					_manager_map[entry.manager->GetName()] = entry;
				} else {
					_log_file->printf("ERROR - Failed to create manager from dynamic module '%s'\n", rel_path.getBuffer());
					error = true;
					return true;
				}
			} else {
				_dynamic_loader.removeModule(name);
				_log_file->printf("ERROR - Failed to find functions 'CreateManager' and 'DestroyManager' in dynamic module '%s'\n", rel_path.getBuffer());
				error = true;
				return true;
			}

		} else {
			_log_file->printf("ERROR - Failed to load dynamic module '%s'\n", rel_path.getBuffer());
			error = true;
			return true;
		}

		return false;
	});

	return !error;
}

bool Game::loadStates(void)
{
	_log_file->writeString("Loading States\n");

	Gaff::JSON state_data;

	if (!state_data.parseFile("./States/states.json")) {
		_log_file->writeString("ERROR - Could not find './States/states.json'\n");
		return false;
	}

	if (!state_data.isObject()) {
		_log_file->writeString("ERROR - './States/states.json' is malformed. Root is not an object.\n");
		return false;
	}

	Gaff::JSON starting_state = state_data["starting_state"];
	Gaff::JSON states = state_data["states"];

	if (!starting_state.isString()) {
		_log_file->writeString("ERROR - './States/states.json' is malformed. 'starting_state' is not a string.\n");
		return false;
	}

	if (!states.isArray()) {
		_log_file->writeString("ERROR - './States/states.json' is malformed. 'states' is not an array.\n");
		return false;
	}

	for (size_t i = 0; i < states.size(); ++i) {
		Gaff::JSON state = states[i];

		if (!state.isObject()) {
			return false;
		}

		Gaff::JSON transitions = state["transitions"];
		Gaff::JSON name = state["name"];

		if (!transitions.isArray()) {
			_log_file->printf("ERROR - './States/states.json' is malformed. Transitions for state entry %i is not an array.\n", i);
			return false;
		}

		if (!name.isString()) {
			_log_file->printf("ERROR - './States/states.json' is malformed. Name for state entry %i is not a string.\n", i);
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
				entry.state = entry.create_func(ProxyAllocator(), *this);

				if (entry.state) {
					entry.transitions.reserve(transitions.size());

					for (size_t j = 0; j < transitions.size(); ++j) {
						Gaff::JSON val = transitions[j];

						if (!val.isInteger()) {
							_log_file->printf("ERROR - './States/states.json' is malformed. Name for state entry %i is not a string.\n", i);
							return false;
						}

						entry.transitions.push((unsigned int)val.getInteger());
					}

					_state_machine.addState(entry);

					if (entry.name == starting_state.getString()) {
						_state_machine.switchState(i);
					}

				} else {
					_log_file->printf("ERROR - Failed to create state '%s' from dynamic module '%s'\n", entry.name.getBuffer(), filename.getBuffer());
					return false;
				}

			} else {
				_dynamic_loader.removeModule(name.getString());
				_log_file->printf("ERROR - Failed to find functions 'CreateState' and 'DestroyState' in dynamic module '%s'\n", filename.getBuffer());
				return false;
			}

		} else {
			_log_file->printf("ERROR - Failed to load dynamic module '%s'\n", filename.getBuffer());
		}
	}

	if (_state_machine.getNextState() == -1) {
		_log_file->writeString("ERROR - 'starting_state' is set to an invalid state name\n");
		return false;
	}

	return true;
}

void Game::run(void)
{
	while (_running) {
		_state_machine.update();
	}
}

ProxyAllocator& Game::getProxyAllocator(void)
{
	return _proxy_allocator;
}

Allocator& Game::getAllocator(void) const
{
	return _allocator;
}

Logger& Game::getLogger(void) const
{
	return _logger;
}

void Game::addTask(Gaff::ITask<ProxyAllocator>* task)
{
	_thread_pool.addTask(task);
}

StateMachine& Game::getStateMachine(void)
{
	return _state_machine;
}

NS_END
