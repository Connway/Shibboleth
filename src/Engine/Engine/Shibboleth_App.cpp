/************************************************************************************
Copyright (C) 2021 by Nicholas LaCroix

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
#include "Shibboleth_EngineAttributesCommon.h"
#include "Shibboleth_LooseFileSystem.h"
#include "Shibboleth_IMainLoop.h"
#include "Shibboleth_IManager.h"
#include "Gen_ReflectionInit.h"
#include <Gaff_CrashHandler.h>
#include <Gaff_Utils.h>
#include <Gaff_JSON.h>
#include <Gaff_File.h>
#include <EAThread/eathread_thread.h>
#include <filesystem>

#ifdef PLATFORM_WINDOWS
	#include <shellapi.h>
#endif

#ifdef INIT_STACKTRACE_SYSTEM
	#include <Gaff_StackTrace.h>
#endif

NS_SHIBBOLETH

App::App(void)
{
	Gaff::InitializeCrashHandler();
	// Set crash handler here

	SetApp(*this);

	_configs = Gaff::JSON::CreateObject();

#ifdef INIT_STACKTRACE_SYSTEM
	GAFF_ASSERT(Gaff::StackTrace::Init());
#endif
}

App::~App(void)
{
}

// Still single-threaded at this point, so ok that we're not locking.
bool App::init(int argc, const char** argv)
{
	const bool application_set_configs = _configs.size() > 0 && _configs["working_dir"].isString();

	// Check if application set working directory.
	if (application_set_configs && !initApp()) {
		return false;
	}

	Gaff::JSON main_config;

	if (main_config.parseFile("cfg/app.cfg")) {
		main_config.forEachInObject([&](const char* key, const Gaff::JSON& value) -> bool {
			_configs.setObject(key, value);
			return false;
		});
	}

	// Load any extra configs and add their values to the _configs object.
	for (int i = 0; i < argc; ++i) {
		const char* const arg = argv[i];
		Gaff::JSON extra_configs;

		if (Gaff::File::CheckExtension(arg, ".cfg")) {
			if (!extra_configs.parseFile(arg)) {
				continue;
			}

			extra_configs.forEachInObject([&](const char* key, const Gaff::JSON& value) -> bool {
				_configs.setObject(key, value);
				return false;
			});

		}
	}

	if (!application_set_configs && !initApp()) {
		return false;
	}

	return initInternal();
}

#ifdef PLATFORM_WINDOWS
// Still single-threaded at this point, so ok that we're not locking.
bool App::init(void)
{
	const bool application_set_configs = _configs.size() > 0 && _configs["working_dir"].isString();

	// Check if application set working directory.
	if (application_set_configs && !initApp()) {
		return false;
	}

	Gaff::JSON main_config;

	if (main_config.parseFile("cfg/app.cfg")) {
		main_config.forEachInObject([&](const char* key, const Gaff::JSON& value) -> bool {
			_configs.setObject(key, value);
			return false;
		});
	}

	int argc = 0;
	const wchar_t* const * const argv = CommandLineToArgvW(GetCommandLineW(), &argc);

	// Load any extra configs and add their values to the _configs object.
	for (int i = 0; i < argc; ++i) {
		const wchar_t* tmp = argv[i];
		CONVERT_STRING(char, arg, tmp);
		Gaff::JSON extra_configs;

		if (!Gaff::File::CheckExtension(arg, ".cfg") || !extra_configs.parseFile(arg)) {
			continue;
		}

		extra_configs.forEachInObject([&](const char* key, const Gaff::JSON& value) -> bool {
			_configs.setObject(key, value);
			return false;
		});
	}

	if (!application_set_configs && !initApp()) {
		return false;
	}

	return initInternal();
}
#endif

void App::run(void)
{
	while (_running) {
		_file_watcher_mgr.update(); // $TODO: Potentially consider threading this.
		_main_loop->update();
	}
}

void App::destroy(void)
{
	if (_main_loop) {
		_main_loop->destroy();
		SHIB_FREET(_main_loop, GetAllocator());
		_main_loop = nullptr;
	}

	const Gaff::JobData thread_deinit_job =
	{
		[](uintptr_t thread_id, void* data) -> void
		{
			App* const app = reinterpret_cast<App*>(data);

			for (const auto& entry : app->_manager_map) {
				entry.second->destroyThread(thread_id);
			}
		},
		static_cast<App*>(&GetApp())
	};

	Gaff::Counter counter = 0;
	_job_pool.addJobsForAllThreads(&thread_deinit_job, 1, counter);
	_job_pool.helpWhileWaiting(counter);

	_job_pool.destroy();

	const Gaff::JSON module_unload_order = _configs["module_unload_order"];
	Vector<Gaff::Hash32> module_hashes;

	// Free managers and shutdown modules in requested to close these first.
	if (!module_unload_order.isNull()) {
		if (module_unload_order.isArray()) {
			module_unload_order.forEachInArray([&](int32_t index, const Gaff::JSON& value) -> bool
			{
				const char* module_name = nullptr;

				if (value.isString()) {
					module_name = value.getString();

				} else if (value.isArray()) {
					if (!value[0].isString() || !value[1].isArray()) {
						LogErrorDefault("module_unload_order[%i] malformed config list!", index);
						return false;
					}

					module_name = value[0].getString();

				} else {
					LogErrorDefault("module_unload_order[%i] is not a string or a config list!", index);
					return false;
				}

				// Find all the managers for this module and free them.
				const auto* const manager_refls = _reflection_mgr.getTypeBucket(Reflection<IManager>::GetHash(), Gaff::FNV1aHash64String(module_name));

				if (manager_refls) {
					for (const Gaff::IReflectionDefinition* ref_def : *manager_refls) {
						const auto it = _manager_map.find(ref_def->getReflectionInstance().getHash());

						if (it != _manager_map.end()) {
							_manager_map.erase(it);
						}
					}
				}

				// Shutdown the module.
				const auto module_shutdown = [&](const char* name) -> void
				{
					DynamicLoader::ModulePtr module = _dynamic_loader.getModule(name);

					if (!module) {
						LogErrorDefault("module_unload_order[%i] module '%s' was not found!", index, name);
						return;
					}

					void (*shutdown_func)(void) = module->getFunc<void (*)(void)>("ShutdownModule");

					if (shutdown_func) {
						module_hashes.emplace_back(Gaff::FNV1aHash32String(name));
						shutdown_func();
					}
				};

				if (value.isString()) {
					module_shutdown(module_name);
					value.freeString(module_name);

				} else if (value.isArray()) {
					value[1].forEachInArray([module_shutdown, index](int32_t flavor_index, const Gaff::JSON& module_flavor) -> bool
					{
						if (!module_flavor.isString()) {
							LogErrorDefault("module_unload_order[%i][%i] is not a string!", index, flavor_index);
							return false;
						}

						const char* const name = module_flavor.getString();
						module_shutdown(name);
						module_flavor.freeString(name);

						return false;
					});
				}

				return false;
			});

		} else {
			LogErrorDefault("'module_unload_order' config option is not an array of strings!");
		}
	}

	_manager_map.clear();

	// Shutdown the modules we haven't already shutdown.
	_dynamic_loader.forEachModule([&](const HashString32<>& module_name, DynamicLoader::ModulePtr module) -> bool
	{
		const auto it = Gaff::Find(module_hashes, module_name.getHash());

		if (it != module_hashes.end()) {
			return false;
		}

		void (*shutdown_func)(void) = module->getFunc<void (*)(void)>("ShutdownModule");

		if (shutdown_func) {
			shutdown_func();
		}

		return false;
	});

#ifdef SHIB_STATIC
	Engine::Gen::ShutdownModulesStatic();
#endif

	_reflection_mgr.destroy();
	_dynamic_loader.clear();

	// Destroy the file system
	if (_fs.file_system_module) {
		_fs.destroy_func(_fs.file_system);
		_fs.file_system_module = nullptr;
	} else if (_fs.file_system) {
		SHIB_FREET(_fs.file_system, GetAllocator());
	}

#ifdef INIT_STACKTRACE_SYSTEM
	Gaff::StackTrace::Destroy();
#endif

	_log_mgr.destroy();
}

const IManager* App::getManager(Gaff::Hash64 name) const
{
	return const_cast<App*>(this)->getManager(name);
}

IManager* App::getManager(Gaff::Hash64 name)
{
	auto it = _manager_map.find(name);
	GAFF_ASSERT(name.getHash() && it != _manager_map.end());
	return it->second.get();
}

//MessageBroadcaster& App::getBroadcaster(void)
//{
//	return _broadcaster;
//}

IFileSystem& App::getFileSystem(void)
{
	return *_fs.file_system;
}

const Gaff::JSON& App::getConfigs(void) const
{
	return _configs;
}

Gaff::JSON& App::getConfigs(void)
{
	return _configs;
}

U8String App::getProjectDirectory(void) const
{
	const Gaff::JSON& wd = _configs["working_dir"];
	return (wd.isString()) ? wd.getString() : ".";
}

const ReflectionManager& App::getReflectionManager(void) const
{
	return _reflection_mgr;
}

ReflectionManager& App::getReflectionManager(void)
{
	return _reflection_mgr;
}

Broadcaster& App::getBroadcaster(void)
{
	return _broadcaster;
}

LogManager& App::getLogManager(void)
{
	return _log_mgr;
}

JobPool& App::getJobPool(void)
{
	return _job_pool;
}

FileWatcherManager& App::getFileWatcherManager(void)
{
	return _file_watcher_mgr;
}

DynamicLoader& App::getDynamicLoader(void)
{
	return _dynamic_loader;
}

bool App::isQuitting(void) const
{
	return !_running;
}

void App::quit(void)
{
	_running = false;
}

bool App::initInternal(void)
{
	const char* log_dir = _configs["log_dir"].getString("./logs");

	size_t prev_index = 0;
	size_t index = Gaff::FindFirstOf(log_dir, '/');

	if (log_dir[0] == '.' && index == 1) {
		index = Gaff::FindFirstOf(log_dir + 2, '/') + 2;
	}

	while (index != U8String::npos) {
		index += prev_index;

		const U8String dir(log_dir, log_dir + prev_index + index - prev_index);

		if (!Gaff::CreateDir(dir.data(), 0777)) {
			return false;
		}

		prev_index = index + 1;
		index = Gaff::FindFirstOf(log_dir + prev_index, '/');
	}

	if (!Gaff::CreateDir(log_dir, 0777)) {
		return false;
	}

	char log_file_with_time[64] = { 0 };
	char time_string[64] = { 0 };

	Gaff::GetCurrentTimeString(time_string, ARRAY_SIZE(time_string), "%Y-%m-%d_%H-%M-%S");
	snprintf(log_file_with_time, ARRAY_SIZE(log_file_with_time), "%s/%s", log_dir, time_string);

	if (!Gaff::CreateDir(log_file_with_time, 0777)) {
		return false;
	}

	SetLogDir(log_file_with_time);

	removeExtraLogs(); // Make sure we don't have more than ten logs per log type

	EA::Thread::SetAllocator(&_thread_allocator);

	if (!_log_mgr.init(log_file_with_time)) {
		return false;
	}

	LogInfoDefault("Initializing...");

	if (!_job_pool.init(static_cast<int32_t>(Gaff::GetNumberOfCores()), App::ThreadInit)) {
		LogErrorDefault("Failed to initialize thread pool.");
		return false;
	}

	const Gaff::JSON read_file_threads = _configs["read_file_threads"];
	_job_pool.addPool(HashStringView32<>(k_read_file_pool_name), read_file_threads.getInt32(k_read_file_pool_default_threads));

	_broadcaster.init();

	if (!loadFileSystem()) {
		return false;
	}

	_reflection_mgr.registerTypeBucket(Gaff::FNV1aHash64Const("Gaff::IAttribute"));
	_reflection_mgr.registerTypeBucket(Gaff::FNV1aHash64Const("IMainLoop"));
	_reflection_mgr.registerTypeBucket(Reflection<IManager>::GetHash());

	for (int32_t mode_count = 0; mode_count < static_cast<int32_t>(InitMode::Count); ++mode_count) {
		const InitMode mode = static_cast<InitMode>(mode_count);

		if (!Engine::Initialize(mode)) {
			LogErrorDefault("Failed to initialize engine reflection.");
			return false;
		}
	}

#ifdef SHIB_STATIC
	for (int32_t mode_count = 0; mode_count < static_cast<int32_t>(InitMode::Count); ++mode_count) {
		const InitMode mode = static_cast<InitMode>(mode_count);

		if (!Engine::Gen::LoadModulesStatic(mode);) {
			return false;
		}
	}
#else
	if (!loadModules()) {
		return false;
	}
#endif

	if (!loadMainLoop()) {
		return false;
	}

	const Gaff::JobData thread_init_job =
	{
		[](uintptr_t thread_id, void* data) -> void
		{
			App* const app = reinterpret_cast<App*>(data);

			for (const auto& entry : app->_manager_map) {
				if (!entry.second->initThread(thread_id)) {
					LogErrorDefault("Failed to initialize thread for '%s'.", entry.second->getReflectionDefinition().getFriendlyName());
					app->quit();
				}
			}
		},
		static_cast<App*>(&GetApp())
	};

	_job_pool.addJobsForAllThreads(&thread_init_job, 1);

	LogInfoDefault("Game Successfully Initialized.");
	return true;
}

// Still single-threaded at this point, so ok that we're not using the spinlock
bool App::loadFileSystem(void)
{
	const Gaff::JSON& lfs = _configs["file_system"];
	U8String fs = "";

	if (lfs.isString()) {
		fs.append_sprintf("%s%s", lfs.getString(), BIT_EXTENSION DYNAMIC_EXTENSION);

		_fs.file_system_module = _dynamic_loader.loadModule(fs.data(), "FileSystem");

		if (!_fs.file_system_module) {
			LogInfoDefault("Failed to find file system '%s'.", fs.data());
			return false;
		}

#ifdef INIT_STACKTRACE_SYSTEM
		Gaff::StackTrace::RefreshModuleList(); // Will fix symbols from DLLs not resolving.
#endif

		LogInfoDefault("Found '%s'. Creating file system.", fs.data());

		InitFileSystemModuleFunc init_func = _fs.file_system_module->getFunc<InitFileSystemModuleFunc>("InitModule");

		if (init_func && !init_func(*this)) {
			LogErrorDefault("Failed to init file system module.");
			return false;
		}

		_fs.destroy_func = _fs.file_system_module->getFunc<FileSystemData::DestroyFileSystemFunc>("DestroyFileSystem");
		_fs.create_func = _fs.file_system_module->getFunc<FileSystemData::CreateFileSystemFunc>("CreateFileSystem");

		if (!_fs.create_func) {
			LogErrorDefault("Failed to find 'CreateFileSystem' in '%s'.", fs.data());
			return false;
		}

		if (!_fs.destroy_func) {
			LogErrorDefault("Failed to find 'DestroyFileSystem' in '%s'.", fs.data());
			return false;
		}

		_fs.file_system = _fs.create_func();

		if (!_fs.file_system) {
			LogErrorDefault("Failed to create file system from '%s'.", fs.data());
			return false;
		}

	} else {
		LogInfoDefault("Defaulting to loose file system.", fs.data());

		_fs.file_system = SHIB_ALLOCT(LooseFileSystem, GetAllocator());

		if (!_fs.file_system) {
			LogErrorDefault("Failed to create loose file system.");
			return false;
		}
	}

	return true;
}

bool App::loadMainLoop(void)
{
	if (_configs["no_main_loop"].isTrue()) {
		return true;
	}

	const Vector<const Gaff::IReflectionDefinition*>* bucket = _reflection_mgr.getTypeBucket(Gaff::FNV1aHash64Const("IMainLoop"));

	if (!bucket || bucket->empty()) {
		LogErrorDefault("Failed to find a class that implements the 'IMainLoop' interface.");
		return false;
	}

	const Gaff::IReflectionDefinition* refl = bucket->front();

	_main_loop = refl->template createT<IMainLoop>(CLASS_HASH(IMainLoop), ProxyAllocator::GetGlobal());

	if (!_main_loop) {
		LogErrorDefault("Failed to construct main loop class '%s'.", refl->getReflectionInstance().getName());
		return false;
	}
	
	if (!_main_loop->init()) {
		LogErrorDefault("Failed to initialize main loop class '%s'.", refl->getReflectionInstance().getName());
		return false;
	}

	return true;
}

bool App::loadModules(void)
{
	if (_configs["no_load_modules"].isTrue()) {
		return true;
	}

	const Gaff::JSON& module_load_order = _configs["module_load_order"];
	const Gaff::JSON& module_dirs = _configs["module_directories"];

	if (!module_dirs.isNull() && !module_dirs.isArray()) {
		LogErrorDefault("'module_directories' config option is not an array of strings.");
		return false;
	}

	if (!module_load_order.isNull() && !module_load_order.isArray()) {
		LogErrorDefault("'module_load_order' config option is not an array.");
		return false;
	}

	for (int32_t mode_count = 0; mode_count < static_cast<int32_t>(InitMode::Count); ++mode_count) {
		const InitMode mode = static_cast<InitMode>(mode_count);

		if (module_dirs.isArray()) {
			const int32_t size = module_dirs.size();

			// Load the rest.
			for (int32_t i = 0; i < size; ++i) {
				const Gaff::JSON& dir = module_dirs[i];
				GAFF_ASSERT(dir.isString());

				for (const auto& dir_entry : std::filesystem::directory_iterator(dir.getString())) {
					if (!dir_entry.is_regular_file()) {
						continue;
					}

					const wchar_t* name = dir_entry.path().c_str();
					CONVERT_STRING(char, temp, name);

					if (!loadModule(temp, mode)) {
						return false;
					}
				}
			}
		}

		// After the first chance to load, all the modules should have registered their job pools.
		if (mode_count == 0) {
			_job_pool.run();
		}
	}

	// Notify all modules that every module has been loaded.
	_dynamic_loader.forEachModule([](const HashString32<>&, DynamicLoader::ModulePtr& module) -> bool
	{
		using AllModulesLoadedNonOwnedFunc = void (*)(void);
		AllModulesLoadedNonOwnedFunc non_owned_func = module->getFunc<AllModulesLoadedNonOwnedFunc>("InitModuleNonOwned");

		if (non_owned_func) {
			non_owned_func();
		}

		return false;
	});

	const bool no_managers = _configs["no_managers"].getBool(false);

	// Create manager instances.
	if (!no_managers) {
		const Vector<const Gaff::IReflectionDefinition*>* manager_bucket = _reflection_mgr.getTypeBucket(Reflection<IManager>::GetHash());

		if (manager_bucket) {
			if (!createManagersInternal(*manager_bucket)) {
				return false;
			}
		}
	}

	Vector<const Gaff::IReflectionDefinition*> already_initialized_managers;

	if (module_load_order.isArray()) {
		const int32_t size = module_load_order.size();

		for (int32_t i = 0; i < size; ++i) {
			const Gaff::JSON& module_row = module_load_order[i];

			if (!module_row.isString()) {
				LogErrorDefault("module_load_order[%i] is not a string.", i);
				continue;
			}

			const char* const module_name = module_row.getString();

			eastl::string_view name_view(module_name);
			const size_t delimeter = name_view.find_last_of('/');

			if (delimeter != eastl::string_view::npos) {
				name_view = name_view.substr(delimeter + 1);
			}

			const Vector<const Gaff::IReflectionDefinition*>* manager_bucket = _reflection_mgr.getTypeBucket(Reflection<IManager>::GetHash(), Gaff::FNV1aHash64(name_view.data(), name_view.size()));

			if (manager_bucket) {
				for (const Gaff::IReflectionDefinition* ref_def : *manager_bucket) {
					if (!_manager_map[ref_def->getReflectionInstance().getHash()]->initAllModulesLoaded()) {
						LogErrorDefault("Failed to initialize manager after all modules loaded '%s'!", ref_def->getReflectionInstance().getName());
						return false;
					}
				}

				already_initialized_managers.insert(already_initialized_managers.end(), manager_bucket->begin(), manager_bucket->end());
			}

			module_row.freeString(module_name);
		}
	}

	// Notify all managers that every module has been loaded.
	for (auto& mgr_pair : _manager_map) {
		if (Gaff::Find(already_initialized_managers, &mgr_pair.second->getReflectionDefinition()) != already_initialized_managers.end()) {
			continue;
		}

		if (!mgr_pair.second->initAllModulesLoaded()) {
			LogErrorDefault("Failed to initialize manager after all modules loaded '%s'!", mgr_pair.second->getReflectionDefinition().getReflectionInstance().getName());
			return false;
		}
	}

	// Notify all modules that every module has been loaded.
	_dynamic_loader.forEachModule([](const HashString32<>&, DynamicLoader::ModulePtr& module) -> bool
	{
		using AllModulesLoadedFunc = void (*)(void);
		AllModulesLoadedFunc aml_func = module->getFunc<AllModulesLoadedFunc>("AllModulesLoaded");

		if (aml_func) {
			aml_func();
		}

		return false;
	});

	return true;
}

bool App::initApp(void)
{
	const Gaff::JSON& wd = _configs["working_dir"];

	if (wd.isString()) {
		if (!Gaff::SetWorkingDir(wd.getString())) {
			LogErrorDefault("Failed to set working directory to '%s'.", wd.getString());
			return false;
		}

		// Set DLL auto-load directory.
#ifdef PLATFORM_WINDOWS
		const char8_t* working_dir_ptr = wd.getString();
		CONVERT_STRING(wchar_t, temp, working_dir_ptr);

		if ((temp[wd.size() - 1] == L'/' || temp[wd.size() - 1] == L'\\')) {
			memcpy(temp + wd.size(), L"bin", sizeof(wchar_t) * 4);
		} else {
			memcpy(temp + wd.size(), L"/bin", sizeof(wchar_t) * 4);
		}

		if (!SetDllDirectory(temp)) {
			LogErrorDefault("Failed to set DLL directory to '%ls'.", temp);
			return false;
		}

	} else {
		if (!SetDllDirectory(L"bin")) {
			LogErrorDefault("Failed to set DLL directory to '%s/bin'.", wd.getString());
			return false;
		}
#endif
	}

	return true;
}

bool App::loadModule(const char* module_path, InitMode mode)
{
	const auto abs_path = std::filesystem::absolute(module_path);
	const wchar_t* name = abs_path.c_str();
	CONVERT_STRING(char, temp, name);

	U8String module_name(temp);
	size_t pos = module_name.find_first_of('\\');

	while (pos != U8String::npos) {
		module_name[pos] = '/';
		pos = module_name.find_first_of('\\');
	}

	pos = module_name.find_last_of('/');
	module_name = module_name.substr(pos + 1, module_name.size() - eastl::CharStrlen("Module" BIT_EXTENSION DYNAMIC_EXTENSION) - (pos + 1));

	DynamicLoader::ModulePtr module = _dynamic_loader.getModule(module_name.data());

	if (!module) {
		module = _dynamic_loader.loadModule(temp, module_name.data());

		if (!module) {
			LogWarningDefault("Failed to find or load dynamic module '%s'.", module_name.data());
			return false;
		}
	}

#ifdef INIT_STACKTRACE_SYSTEM
	Gaff::StackTrace::RefreshModuleList(); // Will fix symbols from DLLs not resolving.
#endif

	InitModuleFunc init_func = module->getFunc<InitModuleFunc>("InitModule");

	if (!init_func) {
		LogErrorDefault("Failed to find function 'InitModule' in dynamic module '%s'.", module_name.data());
		return false;

	} else if (!init_func(*this, mode)) {
		LogErrorDefault("Failed to initialize dynamic module '%s'.", module_name.data());
		return false;
	}

	if (_configs["hot_reload_modules"].isTrue()) {
		const U8String module_file_name(U8String::CtorSprintf(), "%sModule" BIT_EXTENSION DYNAMIC_EXTENSION, module_name.data());
		const Gaff::Flags<Gaff::FileWatcher::NotifyChangeFlag> flags(Gaff::FileWatcher::NotifyChangeFlag::LastWrite);


		for (const auto& dir_entry : std::filesystem::recursive_directory_iterator("../.generated/build/" PLATFORM_NAME)) {
			if (!dir_entry.is_regular_file()) {
				continue;
			}

			const auto abs_file_path = std::filesystem::absolute(dir_entry.path());
			const wchar_t* file_name = abs_file_path.c_str();
			CONVERT_STRING(char, temp_path, file_name);

			if (Gaff::EndsWith(temp_path, "Module" BIT_EXTENSION DYNAMIC_EXTENSION)) {
				temp_path[Gaff::FindLastOf(temp_path, '\\')] = 0;

				size_t index = Gaff::FindFirstOf(temp_path, '\\');
				
				while (index != U8String::npos) {
					temp_path[index] = '/';
					index = Gaff::FindFirstOf(temp_path, '\\');
				}

				if (_file_watcher_mgr.addWatch(temp_path, flags, ModuleChanged)) {
					break;
				} else {
					// $TODO: Log warning.
				}
			}
		}
	}

	return true;
}

void App::removeExtraLogs(void)
{
	const Gaff::JSON log_dir_holder = _configs["log_dir"]; // Need to hold this otherwise the string gets deallocated.
	const char* const log_dir = log_dir_holder.getString("./logs");
	int32_t dir_count = 0;

	for (const auto& dir_entry : std::filesystem::directory_iterator(log_dir)) {
		if (!dir_entry.is_directory()) {
			continue;
		}

		++dir_count;
	}

	if (dir_count <= 10) {
		return;
	}

	for (const auto& dir_entry : std::filesystem::directory_iterator(log_dir)) {
		if (!dir_entry.is_directory()) {
			continue;
		}

		const std::filesystem::path& path = dir_entry.path();
		std::filesystem::remove_all(path);
		--dir_count;

		if (dir_count <= 10) {
			break;
		}
	}
}

bool App::createManagersInternal(const Vector<const Gaff::IReflectionDefinition*>& managers)
{
	ProxyAllocator allocator;

	for (const Gaff::IReflectionDefinition* ref_def : managers) {
		if (hasManager(ref_def->getReflectionInstance().getHash())) {
			continue;
		}

		IManager* const manager = ref_def->createT<IManager>(allocator);

		if (!manager->init()) {
			LogErrorDefault("Failed to initialize manager '%s'!", ref_def->getReflectionInstance().getName());
			SHIB_FREET(manager, GetAllocator());
			return false;
		}

		const Gaff::Hash64 name = ref_def->getReflectionInstance().getHash();

		GAFF_ASSERT(_manager_map.find(name) == _manager_map.end());
		_manager_map[name].reset(manager);
	}

	return true;
}

bool App::hasManager(Gaff::Hash64 name) const
{
	auto it = _manager_map.find(name);
	return it != _manager_map.end();
}

void App::ModuleChanged(const char* path)
{
	if (!Gaff::EndsWith(path, BIT_EXTENSION DYNAMIC_EXTENSION)) {
		return;
	}

	static_cast<App&>(GetApp())._dynamic_loader.forEachModule([&](const HashString32<>& name, const DynamicLoader::ModulePtr& module_ptr) -> bool
	{
		if (Gaff::FindFirstOf(path, name.getBuffer()) == U8String::npos) {
			return false;
		}

		// Copy module to workingdir.
		// Load new module and replace old data structures with new ones.
		// Unload old module and delete file.

		GAFF_REF(module_ptr);

		return false;
	});
}

void App::ThreadInit(uintptr_t /*thread_id*/)
{
	AllocatorThreadInit();
}

NS_END
