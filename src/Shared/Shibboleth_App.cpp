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

#include "Shibboleth_App.h"
#include "Shibboleth_LooseFileSystem.h"
#include "Shibboleth_Utilities.h"
#include "Shibboleth_IManager.h"
#include "Shibboleth_String.h"
#include <Gaff_Utils.h>
#include <Gaff_JSON.h>
#include <regex>

#if defined(SYMBOL_BUILD) && defined(INIT_STACKTRACE_SYSTEM)
	#include <Gaff_StackTrace.h>
#endif

NS_SHIBBOLETH

App::App(void):
	_state_machine(ProxyAllocator()), _logger(ProxyAllocator()),
	_log_file_pair(nullptr), _seed(0), _running(true)
{
	SetApp(*this);
}

App::~App(void)
{
}

// Still single-threaded at this point, so ok that we're not using the spinlock
bool App::init(int argc, char** argv)
{
#if defined(SYMBOL_BUILD) && defined(INIT_STACKTRACE_SYSTEM)
	GAFF_ASSERT(Gaff::StackTrace::Init());
#endif

	while (!_seed) {
		_seed = static_cast<size_t>(time(NULL));
	}

	_cmd_line_args = Gaff::ParseCommandLine<ProxyAllocator>(argc, argv);

	if (!initApp()) {
		return false;
	}

	removeExtraLogs(); // Make sure we don't have more than ten logs per log type

	char log_file_name[64] = { 0 };
	Gaff::GetCurrentTimeString(log_file_name, 64, "Logs/GameLog %Y-%m-%d %H-%M-%S.txt");

	if (!Gaff::CreateDir("./Logs", 0777) || !_logger.openLogFile(log_file_name)) {
		return false;
	}

	_log_file_pair = &_logger.getLogFile(log_file_name);

	_log_file_pair->first.writeString("==================================================\n");
	_log_file_pair->first.writeString("==================================================\n");
	_log_file_pair->first.writeString("Initializing Game...\n");

	Gaff::JSON::SetMemoryFunctions(&ShibbolethAllocate, &ShibbolethFree);
	Gaff::JSON::SetHashSeed(_seed);

	if (!_job_pool.init()) {
		_log_file_pair->first.writeString("ERROR - Failed to initialize thread pool\n");
		return false;
	}

	if (!loadFileSystem()) {
		return false;
	}

#if defined(SYMBOL_BUILD) && defined(INIT_STACKTRACE_SYSTEM)
	Gaff::StackTrace::RefreshModuleList(); // Will fix symbols from DLLs not resolving.
#endif

	if (!loadManagers()) {
		return false;
	}

#if defined(SYMBOL_BUILD) && defined(INIT_STACKTRACE_SYSTEM)
	Gaff::StackTrace::RefreshModuleList(); // Will fix symbols from DLLs not resolving.
#endif

	if (!loadStates()) {
		return false;
	}

#if defined(SYMBOL_BUILD) && defined(INIT_STACKTRACE_SYSTEM)
	Gaff::StackTrace::RefreshModuleList(); // Will fix symbols from DLLs not resolving.
#endif

	_log_file_pair->first.writeString("Game Successfully Initialized\n\n");
	_log_file_pair->first.flush();
	return true;
}

// Still single-threaded at this point, so ok that we're not using the spinlock
bool App::loadFileSystem(void)
{
	if (!_cmd_line_args.hasElementWithKey("lfs") && !_cmd_line_args.hasElementWithKey("loose_file_system")) {
		_fs.file_system_module = _dynamic_loader.loadModule("./FileSystem" BIT_EXTENSION DYNAMIC_EXTENSION, "File System");
	}

	if (_fs.file_system_module) {
		_log_file_pair->first.writeString("Found 'FileSystem" BIT_EXTENSION DYNAMIC_EXTENSION "'. Creating file system\n");

		FileSystemData::InitFileSystemModuleFunc init_func = _fs.file_system_module->getFunc<FileSystemData::InitFileSystemModuleFunc>("InitModule");

		if (init_func && !init_func(*this)) {
			_log_file_pair->first.writeString("ERROR - Failed to init file system module\n");
			return false;
		}

		_fs.destroy_func = _fs.file_system_module->getFunc<FileSystemData::DestroyFileSystemFunc>("DestroyFileSystem");
		_fs.create_func = _fs.file_system_module->getFunc<FileSystemData::CreateFileSystemFunc>("CreateFileSystem");

		if (!_fs.create_func) {
			_log_file_pair->first.writeString("ERROR - Failed to find 'CreateFileSystem' in 'FileSystem" BIT_EXTENSION DYNAMIC_EXTENSION "'\n");
			return false;
		}

		if (!_fs.destroy_func) {
			_log_file_pair->first.writeString("ERROR - Failed to find 'DestroyFileSystem' in 'FileSystem" BIT_EXTENSION DYNAMIC_EXTENSION "'\n");
			return false;
		}

		_fs.file_system = _fs.create_func();

		if (!_fs.file_system) {
			_log_file_pair->first.writeString("ERROR - Failed to create file system from 'FileSystem" BIT_EXTENSION DYNAMIC_EXTENSION "'\n");
			return false;
		}

	} else {
		_log_file_pair->first.writeString("Could not find 'FileSystem" BIT_EXTENSION DYNAMIC_EXTENSION "' defaulting to loose file system\n");
		_fs.file_system = GetAllocator()->template allocT<LooseFileSystem>();

		if (!_fs.file_system) {
			_log_file_pair->first.writeString("ERROR - Failed to create loose file system\n");
			return false;
		}
	}

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

		// If it's not a dynamic module, just skip over it.
		if (!Gaff::File::CheckExtension(name, DYNAMIC_EXTENSION)) {
			return false;

		// It is a dynamic module, but not compiled for our architecture.
		// Or not compiled in our build mode. Just skip over it.
		} else if (!Gaff::File::CheckExtension(name, BIT_EXTENSION DYNAMIC_EXTENSION)) {
			return false;
		}

#ifdef PLATFORM_WINDOWS
		DynamicLoader::ModulePtr module = _dynamic_loader.loadModule(("../" + rel_path).getBuffer(), name);
#else
		DynamicLoader::ModulePtr module = _dynamic_loader.loadModule(rel_path.getBuffer(), name);
#endif

		if (module) {
			ManagerEntry::InitManagerModuleFunc init_func = module->getFunc<ManagerEntry::InitManagerModuleFunc>("InitModule");

			if (!init_func) {
				_log_file_pair->first.printf("ERROR - Failed to find function 'InitModule' in dynamic module '%s'\n", rel_path.getBuffer());
				error = true;
				return true;

			} else if (!init_func(*this)) {
				_log_file_pair->first.printf("ERROR - Failed to initialize '%s'\n", rel_path.getBuffer());
				error = true;
				return true;
			}

			ManagerEntry::GetNumManagersFunc num_mgrs_func = module->getFunc<ManagerEntry::GetNumManagersFunc>("GetNumManagers");

			if (!num_mgrs_func) {
				_dynamic_loader.removeModule(name);
				_log_file_pair->first.printf("ERROR - Failed to find function 'GetNumManagers' in dynamic module '%s'\n", rel_path.getBuffer());
				error = true;
				return true;
			}

			unsigned int num_managers = num_mgrs_func();

			for (unsigned int i = 0; i < num_managers; ++i) {
				ManagerEntry entry = { module, nullptr, nullptr, nullptr, i };
				entry.create_func = module->getFunc<ManagerEntry::CreateManagerFunc>("CreateManager");
				entry.destroy_func = module->getFunc<ManagerEntry::DestroyManagerFunc>("DestroyManager");

				if (entry.create_func && entry.destroy_func) {
					entry.manager = entry.create_func(i);
					entry.manager_id = i;

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
			_log_file_pair->first.printf("ERROR - Failed to load dynamic module '%s' for reason '%s'\n", rel_path.getBuffer(), Gaff::DynamicModule::GetErrorString());
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

	IFile* states_file = _fs.file_system->openFile("States/states.json");

	if (!states_file) {
		_log_file_pair->first.writeString("ERROR - Could not find 'States/states.json'.\n");
		return false;
	}

	Gaff::JSON state_data;

	if (!state_data.parse(states_file->getBuffer())) {
		_log_file_pair->first.writeString("ERROR - When parsing 'States/states.json'.\n");
		_fs.file_system->closeFile(states_file);
		return false;
	}

	_fs.file_system->closeFile(states_file);

	if (!state_data.isObject()) {
		_log_file_pair->first.writeString("ERROR - 'States/states.json' is malformed. Root is not an object.\n");
		return false;
	}

	Gaff::JSON starting_state = state_data["starting_state"];
	Gaff::JSON states = state_data["states"];

	if (!starting_state.isString()) {
		_log_file_pair->first.writeString("ERROR - 'States/states.json' is malformed. 'starting_state' is not a string.\n");
		return false;
	}

	if (!states.isArray()) {
		_log_file_pair->first.writeString("ERROR - 'States/states.json' is malformed. 'states' is not an array.\n");
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
			_log_file_pair->first.printf("ERROR - 'States/states.json' is malformed. Transitions for state entry %zu is not an array.\n", i);
			return false;
		}

		if (!module_name.isString()) {
			_log_file_pair->first.printf("ERROR - 'States/states.json' is malformed. Module name for state entry %zu is not a string.\n", i);
			return false;
		}

		if (!state_name.isString()) {
			_log_file_pair->first.printf("ERROR - 'States/states.json' is malformed. Name for state entry %zu is not a string.\n", i);
			return false;
		}

#ifdef PLATFORM_WINDOWS
		AString filename("../States/");
#else
		AString filename("./States/");
#endif
		filename += module_name.getString();
		filename += BIT_EXTENSION DYNAMIC_EXTENSION;

		DynamicLoader::ModulePtr module = _dynamic_loader.loadModule(filename.getBuffer(), module_name.getString());

		if (module) {
			StateMachine::StateEntry::InitStateModuleFunc init_func = module->getFunc<StateMachine::StateEntry::InitStateModuleFunc>("InitModule");

			if (!init_func) {
				_log_file_pair->first.printf("ERROR - Failed to find function 'InitModule' in dynamic module '%s'\n", filename.getBuffer());
				return false;
			} else if (!init_func(*this)) {
				_log_file_pair->first.printf("ERROR - Failed to initialize '%s'\n", filename.getBuffer());
				return true;
			}

			StateMachine::StateEntry::GetNumStatesFunc num_states_func = module->getFunc<StateMachine::StateEntry::GetNumStatesFunc>("GetNumStates");
			StateMachine::StateEntry::GetStateNameFunc get_state_name_func = module->getFunc<StateMachine::StateEntry::GetStateNameFunc>("GetStateName");

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

			StateMachine::StateEntry entry = { AString(state_name.getString()), nullptr, nullptr, nullptr, state_id, j };
			entry.create_func = module->getFunc<StateMachine::StateEntry::CreateStateFunc>("CreateState");
			entry.destroy_func = module->getFunc<StateMachine::StateEntry::DestroyStateFunc>("DestroyState");

			if (entry.create_func && entry.destroy_func) {
				entry.state = entry.create_func(j);
				++state_id;

				if (entry.state) {
					entry.state->_transitions.reserve(static_cast<unsigned int>(transitions.size()));

					for (size_t k = 0; k < transitions.size(); ++k) {
						Gaff::JSON val = transitions[k];

						if (!val.isInteger()) {
							_log_file_pair->first.printf("ERROR - 'States/states.json' is malformed. Name for state entry %zu is not a string.\n", i);
							return false;
						}

						entry.state->_transitions.push(static_cast<unsigned int>(val.getInteger()));
					}

					if (!entry.state->init(_state_machine.getNumStates())) {
						_log_file_pair->first.printf("ERROR - Failed to initialize state '%s'\n", state_name.getString());
						return false;
					}

					_state_machine.addState(entry);
					_log_file_pair->first.printf("Loaded state '%s'\n", state_name.getString());

					if (entry.name == starting_state.getString()) {
						_state_machine.switchState(static_cast<unsigned int>(i));
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
			_log_file_pair->first.printf("ERROR - Failed to load dynamic module '%s' for reason '%s'\n", filename.getBuffer(), Gaff::DynamicModule::GetErrorString());
		}
	}

	if (_state_machine.getNextState() == UINT_FAIL) {
		_log_file_pair->first.writeString("ERROR - 'starting_state' is set to an invalid state name\n");
		return false;
	}

	return true;
}

bool App::initApp(void)
{
	if (_cmd_line_args.hasElementWithKey("working_dir")) {
		const AString& working_dir = _cmd_line_args["working_dir"];

		if (!Gaff::SetWorkingDir(working_dir.getBuffer())) {
			//_log_file_pair->first.printf("ERROR - Failed to set working directory to '%s'.\n", working_dir.getBuffer());
			return false;
		}

	// Set DLL auto-load directory.
#ifdef PLATFORM_WINDOWS
	#ifdef _UNICODE
		wchar_t temp[1024] = { 0 };
		Gaff::ConvertToUTF16(temp, working_dir.getBuffer(), working_dir.size());
	#else
		char temp[1024] = { 0 };
		memcpy(temp, working_dir.getBuffer(), working_dir.size());
	#endif

		if ((temp[working_dir.size() - 1] == TEXT('/') || temp[working_dir.size() - 1] == TEXT('\\'))) {
			memcpy(temp + working_dir.size(), TEXT("bin"), sizeof(TCHAR));
		} else {
			memcpy(temp + working_dir.size(), TEXT("/bin"), sizeof(TCHAR));
		}

		SetDllDirectory(temp);

	} else {
		SetDllDirectory(TEXT("bin"));
	}
#else
	}
#endif

	return true;
}

void App::removeExtraLogs(void)
{
	unsigned int callstack_log_count = 0;
	unsigned int alloc_log_count = 0;
	unsigned int game_log_count = 0;

	Gaff::ForEachTypeInDirectory<Gaff::FDT_RegularFile>("./Logs", [&](const char* name, size_t) -> bool
	{
		if (std::regex_match(name, std::regex("GameLog.+\.txt"))) {
			++game_log_count;
		} else if (std::regex_match(name, std::regex("AllocationLog.+\.txt"))) {
			++alloc_log_count;
		} else if (std::regex_match(name, std::regex("CallstackLog.+\.txt"))) {
			++callstack_log_count;
		}

		return false;
	});

	unsigned int callstack_logs_delete = (callstack_log_count > 10) ? callstack_log_count - 10 : 0;
	unsigned int alloc_logs_delete = (alloc_log_count > 10) ? alloc_log_count - 10 : 0;
	unsigned int game_logs_delete = (game_log_count > 10) ? game_log_count - 10 : 0;
	callstack_log_count = alloc_log_count = game_log_count = 0;
	AString temp;

	Gaff::ForEachTypeInDirectory<Gaff::FDT_RegularFile>("./Logs", [&](const char* name, size_t) -> bool
	{
		if (std::regex_match(name, std::regex("GameLog.+\.txt")) && game_log_count < game_logs_delete) {
			temp = AString("./Logs/") + name;
			std::remove(temp.getBuffer());
			++game_log_count;

		} else if (std::regex_match(name, std::regex("AllocationLog.+\.txt")) && alloc_log_count < alloc_logs_delete) {
			temp = AString("./Logs/") + name;
			std::remove(temp.getBuffer());
			++alloc_log_count;

		} else if (std::regex_match(name, std::regex("CallstackLog.+\.txt")) && callstack_log_count < callstack_logs_delete) {
			temp = AString("./Logs/") + name;
			std::remove(temp.getBuffer());
			++callstack_log_count;
		}

		return false;
	});
}

void App::run(void)
{
	while (_running) {
		_broadcaster.update();
		_state_machine.update();
	}
}

void App::destroy(void)
{
	_job_pool.destroy();
	_state_machine.clear();

	for (ManagerMap::Iterator it = _manager_map.begin(); it != _manager_map.end(); ++it) {
		it->destroy_func(it->manager, it->manager_id);
	}

	_manager_map.clear();
	_logger.destroy();

	_dynamic_loader.forEachModule([](DynamicLoader::ModulePtr module) -> bool
	{
		void(*shutdown_func)(void) = module->getFunc<void(*)(void)>("ShutdownModule");

		if (shutdown_func) {
			shutdown_func();
		}

		return false;
	});

	_dynamic_loader.clear();

	// Destroy the file system
	if (_fs.file_system_module) {
		_fs.destroy_func(_fs.file_system);
		_fs.file_system_module = nullptr;
	} else if (_fs.file_system) {
		GetAllocator()->freeT(_fs.file_system);
	}

#if defined(SYMBOL_BUILD) && defined(INIT_STACKTRACE_SYSTEM)
	Gaff::StackTrace::Destroy();
#endif
}

const IManager* App::getManager(const AHashString& name) const
{
	GAFF_ASSERT(name.size() && _manager_map.indexOf(name) != SIZE_T_FAIL);
	return _manager_map[name].manager;
}

const IManager* App::getManager(const AString& name) const
{
	GAFF_ASSERT(name.size() && _manager_map.indexOf(name) != SIZE_T_FAIL);
	return _manager_map[name].manager;
}

const IManager* App::getManager(const char* name) const
{
	GAFF_ASSERT(name && _manager_map.indexOf(name) != SIZE_T_FAIL);
	return _manager_map[name].manager;
}

IManager* App::getManager(const AHashString& name)
{
	GAFF_ASSERT(name.size() && _manager_map.indexOf(name) != SIZE_T_FAIL);
	return _manager_map[name].manager;
}

IManager* App::getManager(const AString& name)
{
	GAFF_ASSERT(name.size() && _manager_map.indexOf(name) != SIZE_T_FAIL);
	return _manager_map[name].manager;
}

IManager* App::getManager(const char* name)
{
	GAFF_ASSERT(name && _manager_map.indexOf(name) != SIZE_T_FAIL);
	return _manager_map[name].manager;
}

MessageBroadcaster& App::getBroadcaster(void)
{
	return _broadcaster;
}

const Array<unsigned int>& App::getStateTransitions(unsigned int state_id)
{
	return _state_machine.getTransitions(state_id);
}

unsigned int App::getStateID(const char* name)
{
	return _state_machine.getStateID(name);
}

void App::switchState(unsigned int state)
{
	_state_machine.switchState(state);
}

IFileSystem* App::getFileSystem(void)
{
	return _fs.file_system;
}

const HashMap<AHashString, AString>& App::getCmdLine(void) const
{
	return _cmd_line_args;
}

HashMap<AHashString, AString>& App::getCmdLine(void)
{
	return _cmd_line_args;
}

JobPool& App::getJobPool(void)
{
	return _job_pool;
}

void App::getWorkerThreadIDs(Array<unsigned int>& out) const
{
	out.resize(_job_pool.getNumTotalThreads());
	_job_pool.getThreadIDs(out.getArray());
}

void App::helpUntilNoJobs(void)
{
	_job_pool.helpUntilNoJobs();
}

void App::doAJob(void)
{
	_job_pool.doAJob();
}

void App::addLogCallback(const LogManager::LogCallback& callback)
{
	_logger.addLogCallback(callback);
}

void App::removeLogCallback(const LogManager::LogCallback& callback)
{
	_logger.removeLogCallback(callback);
}

void App::notifyLogCallbacks(const char* message, LogManager::LOG_TYPE type)
{
	_logger.notifyLogCallbacks(message, type);
}

LogManager::FileLockPair& App::getGameLogFile(void)
{
	return *_log_file_pair;
}

DynamicLoader::ModulePtr App::loadModule(const char* filename, const char* name)
{
	return _dynamic_loader.loadModule(filename, name);
}

size_t App::getSeed(void) const
{
	return _seed;
}

bool App::isQuitting(void) const
{
	return !_running;
}

void App::quit(void)
{
	_running = false;
}

NS_END
