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

#include "Shibboleth_App.h"
#include "Shibboleth_Allocator.h"
#include "Shibboleth_IManager.h"
#include "Shibboleth_String.h"
#include <Gaff_Utils.h>
#include <Gaff_JSON.h>
#include <iostream>

NS_SHIBBOLETH

// Have to pass in the correct ProxyAllocator, as we have not registered our allocator globally yet
App::App(void):
	_seed(0), _running(true)
{
	SetAllocator(&_allocator);
}

App::~App(void)
{
	for (ManagerMap::Iterator it = _manager_map.begin(); it != _manager_map.end(); ++it) {
		it->destroy_func(it->manager, it->manager_id);
	}

	_manager_map.clear();
	_broadcaster.destroy();
	_state_machine.clear();
	_thread_pool.destroy();
	_logger.destroy();

	_dynamic_loader.forEachModule([](DynamicLoader::ModulePtr module) -> bool
	{
		void (*shutdown_func)(void) = module->GetFunc<void (*)(void)>("ShutdownModule");

		if (shutdown_func) {
			shutdown_func();
		}

		return false;
	});

	_dynamic_loader.clear();
}

// Still single-threaded at this point, so ok that we're not using the spinlock
bool App::init(void)
{
	if (!_logger.init()) {
		return false;
	}

	while (!_seed) {
		_seed = (size_t)time(NULL);
	}

	char log_file_name[64] = { 0 };
	Gaff::GetCurrentTimeString(log_file_name, 64, "Logs/GameLog %m-%d-%Y %H-%M-%S.txt");

	if (!Gaff::CreateDir("./Logs", 0777) || !_logger.openLogFile(log_file_name)) {
		return false;
	}

	_log_file_pair = &_logger.getLogFile(log_file_name);

	_log_file_pair->first.writeString("==================================================\n");
	_log_file_pair->first.writeString("==================================================\n");
	_log_file_pair->first.writeString("Initializing Game...\n");

	Gaff::JSON::SetMemoryFunctions(&ShibbolethAllocate, &ShibbolethFree);
	Gaff::JSON::SetHashSeed(_seed);

	if (!_thread_pool.init()) {
		_log_file_pair->first.writeString("ERROR - Failed to initialize thread pool\n");
		return false;
	}

	if (!_broadcaster.init()) {
		_log_file_pair->first.writeString("ERROR - Failed to initialize message broadcaster\n");
		return false;
	}

	if (!loadManagers()) {
		return false;
	}

	if (!loadStates()) {
		return false;
	}

	_log_file_pair->first.writeString("Game Successfully Initialized\n\n");
	_log_file_pair->first.flush();
	return true;
}

// Still single-threaded at this point, so ok that we're not using the spinlock
bool App::loadManagers(void)
{
	_log_file_pair->first.writeString("Loading Managers...\n");

	bool error = false;

	// Load managers from DLLs
	Gaff::ForEachTypeInDirectory<Gaff::FDT_RegularFile>("./Managers", [&](const char* name, size_t) -> bool
	{
		AString rel_path = AString("./Managers/") + name;

		// Error out if it's not a dynamic module
		if (!Gaff::File::checkExtension(name, DYNAMIC_EXTENSION)) {
			_log_file_pair->first.printf("ERROR - '%s' is not a dynamic module.\n", rel_path.getBuffer());
			error = true;
			return true;

		// It is a dynamic module, but not compiled for our architecture.
		// Or not compiled in our build mode. Just skip over it.
		} else if (!Gaff::File::checkExtension(name, BIT_EXTENSION DYNAMIC_EXTENSION)) {
			return false;
		}

		DynamicLoader::ModulePtr module = _dynamic_loader.loadModule(rel_path.getBuffer(), name);

		if (module) {
			ManagerEntry::InitManagerModuleFunc init_func = module->GetFunc<ManagerEntry::InitManagerModuleFunc>("InitModule");

			if (!init_func) {
				//_dynamic_loader.removeModule(name);
				_log_file_pair->first.printf("ERROR - Failed to find function 'InitModule' in dynamic module '%s'\n", rel_path.getBuffer());
				error = true;
				return true;
			} else if (!init_func(*this)) {
				//_dynamic_loader.removeModule(name);
				_log_file_pair->first.printf("ERROR - Failed to initialize '%s'\n", rel_path.getBuffer());
				error = true;
				return true;
			}

			ManagerEntry::GetNumManagersFunc num_mgrs_func = module->GetFunc<ManagerEntry::GetNumManagersFunc>("GetNumManagers");

			if (!num_mgrs_func) {
				_dynamic_loader.removeModule(name);
				_log_file_pair->first.printf("ERROR - Failed to find function 'GetNumManagers' in dynamic module '%s'\n", rel_path.getBuffer());
				error = true;
				return true;
			}

			unsigned int num_managers = num_mgrs_func();

			for (unsigned int i = 0; i < num_managers; ++i) {
				ManagerEntry entry = { module, nullptr, nullptr, nullptr, i };
				entry.create_func = module->GetFunc<ManagerEntry::CreateManagerFunc>("CreateManager");
				entry.destroy_func = module->GetFunc<ManagerEntry::DestroyManagerFunc>("DestroyManager");

				if (entry.create_func && entry.destroy_func) {
					entry.manager = entry.create_func(i);

					if (entry.manager) {
						_manager_map[entry.manager->getName()] = entry;
						_log_file_pair->first.printf("Loaded manager '%s'\n", entry.manager->getName());

					} else {
						_log_file_pair->first.printf("ERROR - Failed to create manager from dynamic module '%s'\n", rel_path.getBuffer());
						error = true;
						return true;
					}
				} else {
					_dynamic_loader.removeModule(name);
					_log_file_pair->first.printf("ERROR - Failed to find functions 'CreateManager' and/or 'DestroyManager' in dynamic module '%s'\n", rel_path.getBuffer());
					error = true;
					return true;
				}
			}

		} else {
			_log_file_pair->first.printf("ERROR - Failed to load dynamic module '%s'\n", rel_path.getBuffer());
			error = true;
			return true;
		}

		return false;
	});

	if (!error) {
		for (auto it = _manager_map.begin(); it != _manager_map.end(); ++it) {
			(*it).manager->allManagersCreated();
		}
	}

	return !error;
}

// Still single-threaded at this point, so ok that we're not using the spinlock
bool App::loadStates(void)
{
	_log_file_pair->first.writeString("Loading States...\n");

	Gaff::JSON state_data;

	if (!state_data.parseFile("./States/states.json")) {
		_log_file_pair->first.writeString("ERROR - Could not find './States/states.json' or there was a parsing error.\n");
		return false;
	}

	if (!state_data.isObject()) {
		_log_file_pair->first.writeString("ERROR - './States/states.json' is malformed. Root is not an object.\n");
		return false;
	}

	Gaff::JSON starting_state = state_data["starting_state"];
	Gaff::JSON states = state_data["states"];

	if (!starting_state.isString()) {
		_log_file_pair->first.writeString("ERROR - './States/states.json' is malformed. 'starting_state' is not a string.\n");
		return false;
	}

	if (!states.isArray()) {
		_log_file_pair->first.writeString("ERROR - './States/states.json' is malformed. 'states' is not an array.\n");
		return false;
	}

	unsigned int state_id = 0;

	for (size_t i = 0; i < states.size(); ++i) {
		Gaff::JSON state = states[i];

		if (!state.isObject()) {
			return false;
		}

		Gaff::JSON transitions = state["transitions"];
		Gaff::JSON module_name = state["module"];
		Gaff::JSON state_name = state["name"];

		if (!transitions.isArray()) {
			_log_file_pair->first.printf("ERROR - './States/states.json' is malformed. Transitions for state entry %i is not an array.\n", i);
			return false;
		}

		if (!module_name.isString()) {
			_log_file_pair->first.printf("ERROR - './States/states.json' is malformed. Module name for state entry %i is not a string.\n", i);
			return false;
		}

		if (!state_name.isString()) {
			_log_file_pair->first.printf("ERROR - './States/states.json' is malformed. Name for state entry %i is not a string.\n", i);
			return false;
		}

		AString filename("./States/");
		filename += module_name.getString();
		filename += BIT_EXTENSION DYNAMIC_EXTENSION;

		DynamicLoader::ModulePtr module = _dynamic_loader.loadModule(filename, module_name.getString());

		if (module) {
			StateMachine::StateEntry::InitStateModuleFunc init_func = module->GetFunc<StateMachine::StateEntry::InitStateModuleFunc>("InitModule");

			if (!init_func) {
				_log_file_pair->first.printf("ERROR - Failed to find function 'InitModule' in dynamic module '%s'\n", filename.getBuffer());
				return false;
			} else if (!init_func(*this)) {
				_log_file_pair->first.printf("ERROR - Failed to initialize '%s'\n", filename.getBuffer());
				return true;
			}

			StateMachine::StateEntry::GetNumStatesFunc num_states_func = module->GetFunc<StateMachine::StateEntry::GetNumStatesFunc>("GetNumStates");
			StateMachine::StateEntry::GetStateNameFunc get_state_name_func = module->GetFunc<StateMachine::StateEntry::GetStateNameFunc>("GetStateName");

			if (!num_states_func || !get_state_name_func) {
				_log_file_pair->first.printf("ERROR - Failed to find functions 'GetNumStates' and/or 'GetStateName' in dynamic module '%s'\n", filename.getBuffer());
				return false;
			}

			unsigned int num_states = num_states_func();
			unsigned int j = 0;

			for (; j < num_states; ++j) {
				if (!strcmp(state_name.getString(), get_state_name_func(j))) {
					break;
				}
			}

			if (j == num_states) {
				_log_file_pair->first.printf("ERROR - Failed to find state with name '%s' in dynamic module '%s'\n", state_name.getString(), filename.getBuffer());
				return false;
			}

			StateMachine::StateEntry entry = { AString(state_name.getString()), nullptr, nullptr, nullptr, state_id };
			entry.create_func = module->GetFunc<StateMachine::StateEntry::CreateStateFunc>("CreateState");
			entry.destroy_func = module->GetFunc<StateMachine::StateEntry::DestroyStateFunc>("DestroyState");

			if (entry.create_func && entry.destroy_func) {
				entry.state = entry.create_func(state_id);
				++state_id;

				if (entry.state) {
					entry.state->_transitions.reserve((unsigned int)transitions.size());

					for (size_t k = 0; k < transitions.size(); ++k) {
						Gaff::JSON val = transitions[k];

						if (!val.isInteger()) {
							_log_file_pair->first.printf("ERROR - './States/states.json' is malformed. Name for state entry %i is not a string.\n", i);
							return false;
						}

						entry.state->_transitions.push((unsigned int)val.getInteger());
					}

					if (!entry.state->init(_state_machine.getNumStates())) {
						_log_file_pair->first.printf("ERROR - Failed to initialize state '%s'\n", state_name.getString());
						return false;
					}

					_state_machine.addState(entry);
					_log_file_pair->first.printf("Loaded state '%s'\n", state_name.getString());

					if (entry.name == starting_state.getString()) {
						_state_machine.switchState((unsigned int)i);
					}

				} else {
					_log_file_pair->first.printf("ERROR - Failed to create state '%s' from dynamic module '%s'\n", entry.name.getBuffer(), filename.getBuffer());
					return false;
				}

			} else {
				_log_file_pair->first.printf("ERROR - Failed to find functions 'CreateState' and 'DestroyState' in dynamic module '%s'\n", filename.getBuffer());
				return false;
			}

		} else {
			_log_file_pair->first.printf("ERROR - Failed to load dynamic module '%s'\n", filename.getBuffer());
		}
	}

	if (_state_machine.getNextState() == -1) {
		_log_file_pair->first.writeString("ERROR - 'starting_state' is set to an invalid state name\n");
		return false;
	}

	return true;
}

void App::run(void)
{
	while (_running) {
		_broadcaster.update(_thread_pool);
		_state_machine.update();
	}
}

ThreadPool& App::getThreadPool(void)
{
	return _thread_pool;
}

Allocator& App::getAllocator(void)
{
	return _allocator;
}

LogManager& App::getLogManager(void)
{
	return _logger;
}

void App::addTask(Gaff::TaskPtr<ProxyAllocator>& task)
{
	_thread_pool.addTask(task);
}

MessageBroadcaster& App::getBroadcaster(void)
{
	return _broadcaster;
}

StateMachine& App::getStateMachine(void)
{
	return _state_machine;
}

DynamicLoader& App::getDynamicLoader(void)
{
	return _dynamic_loader;
}

LogManager::FileLockPair& App::getGameLogFile(void)
{
	return *_log_file_pair;
}

size_t App::getSeed(void) const
{
	return _seed;
}

void App::quit(void)
{
	_running = false;
}

NS_END
