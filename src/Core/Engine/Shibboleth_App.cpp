/************************************************************************************
Copyright (C) 2023 by Nicholas LaCroix

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
#include "Shibboleth_AppConfigs.h"
#include "Shibboleth_IMainLoop.h"
#include "Shibboleth_IManager.h"
#include "Gen_StaticReflectionInit.h"
#include <Gaff_CrashHandler.h>
#include <Gaff_Utils.h>
#include <Gaff_JSON.h>
#include <Gaff_File.h>
#include <eathread/eathread_thread.h>
#include <filesystem>

#ifndef SHIB_STATIC
	#define SHIB_STATIC
	#include "Gen_ReflectionInit.h"
	#undef SHIB_STATIC
#else
	#include "Gen_ReflectionInit.h"
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
	Gaff::UninitializeCrashHandler();
}

// Still single-threaded at this point, so ok that we're not locking.
bool App::init(int argc, const char** argv)
{
	const bool application_set_configs = _configs.size() > 0 && _configs.getObject(k_config_app_working_dir).isString();

	// Check if application set working directory.
	if (application_set_configs && !initApp()) {
		return false;
	}

	Gaff::JSON main_config;

	if (main_config.parseFile(u8"cfg/app.cfg")) {
		main_config.forEachInObject([&](const char8_t* key, const Gaff::JSON& value) -> bool {
			_configs.setObject(key, value);
			return false;
		});
	}

	// Load any extra configs and add their values to the _configs object.
	for (int i = 0; i < argc; ++i) {
		const char* arg = argv[i];
		Gaff::JSON extra_configs;

		if (Gaff::EndsWith(arg, ".cfg")) {
			CONVERT_STRING(char8_t, temp_arg, arg);

			if (!extra_configs.parseFile(temp_arg)) {
				continue;
			}

			extra_configs.forEachInObject([&](const char8_t* key, const Gaff::JSON& value) -> bool {
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

void App::run(void)
{
	while (_running) {
		//_file_watcher_mgr.update(); // $TODO: Potentially consider threading this.
		_main_loop->update();
	}
}

void App::destroy(void)
{
	if (_job_pool.isPaused()) {
		_job_pool.run();
	}

	const EA::Thread::ThreadId thread_id = EA::Thread::GetThreadId();

	_job_pool.waitForAllJobsToFinish(thread_id);
	_job_pool.destroy();

	for (const auto& entry : _manager_map) {
		entry.second->destroyThread((uintptr_t)&thread_id);
	}

	if (_main_loop) {
		_main_loop->destroy();
		SHIB_FREET(_main_loop, GetAllocator());
		_main_loop = nullptr;
	}

	const Gaff::JSON module_unload_order = _configs.getObject(k_config_module_unload_order);
	Vector<Gaff::Hash32> module_hashes;

	// Free managers and shutdown modules in requested to close these first.
	if (!module_unload_order.isNull()) {
		if (module_unload_order.isArray()) {
			module_unload_order.forEachInArray([&](int32_t index, const Gaff::JSON& value) -> bool
			{
				const char8_t* module_name = nullptr;

				if (value.isString()) {
					module_name = value.getString();

				} else {
					LogErrorDefault("module_unload_order[%i] is not a string!", index);
					return false;
				}

				const Gaff::Hash64 module_name_hash = Gaff::FNV1aHash64String(module_name);
				value.freeString(module_name);

				// Find all the managers for this module and free them.
				const auto* const manager_refls = _reflection_mgr.getTypeBucket(Refl::Reflection<IManager>::GetNameHash(), module_name_hash);

				if (manager_refls) {
					for (const Refl::IReflectionDefinition* ref_def : *manager_refls) {
						const auto it = _manager_map.find(ref_def->getReflectionInstance().getNameHash());

						if (it != _manager_map.end()) {
							_manager_map.erase(it);
						}
					}
				}

				const auto module_ptr_it = _module_map.find(module_name_hash);

				if (module_ptr_it != _module_map.end()) {
					module_ptr_it->second->shutdown();
				}

				return false;
			});

		} else {
			LogErrorDefault("'module_unload_order' config option is not an array of strings!");
		}
	}

	_manager_map.clear();
	_module_map.clear();
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
	const Gaff::JSON& wd = _configs.getObject(k_config_app_working_dir);
	return (wd.isString()) ? wd.getString() : u8".";
}

const ReflectionManager& App::getReflectionManager(void) const
{
	return _reflection_mgr;
}

ReflectionManager& App::getReflectionManager(void)
{
	return _reflection_mgr;
}

LogManager& App::getLogManager(void)
{
	return _log_mgr;
}

JobPool& App::getJobPool(void)
{
	return _job_pool;
}

//FileWatcherManager& App::getFileWatcherManager(void)
//{
//	return _file_watcher_mgr;
//}

DynamicLoader& App::getDynamicLoader(void)
{
	return _dynamic_loader;
}

Broadcaster& App::getBroadcaster(void)
{
	return _broadcaster;
}

bool App::isQuitting(void) const
{
	return !_running;
}

void App::quit(void)
{
	_running = false;
}

#ifdef SHIB_RUNTIME_VAR_ENABLED
RuntimeVarManager& App::getRuntimeVarManager(void)
{
	return _runtime_var_mgr;
}
#endif

bool App::initInternal(void)
{
	const char8_t* const log_dir = _configs.getObject(k_config_app_log_dir).getString(k_config_app_default_log_dir);

	size_t prev_index = 0;
	size_t index = Gaff::Find(log_dir, u8'/');

	if (log_dir[0] == '.' && index == 1) {
		index = Gaff::Find(log_dir + 2, u8'/') + 2;
	}

	while (index != GAFF_SIZE_T_FAIL) {
		index += prev_index;

		const U8String dir(log_dir, log_dir + prev_index + index - prev_index);

		if (!Gaff::CreateDir(dir.data(), 0777)) {
			return false;
		}

		prev_index = index + 1;
		index = Gaff::Find(log_dir + prev_index, u8'/');
	}

	if (!Gaff::CreateDir(log_dir, 0777)) {
		return false;
	}

	char8_t time_string[64] = { 0 };
	Gaff::GetCurrentTimeString(time_string, std::size(time_string), u8"%Y-%m-%d_%H-%M-%S");

	const U8String log_file_with_time(U8String::CtorSprintf(), u8"%s/%s", log_dir, time_string);

	if (!Gaff::CreateDir(log_file_with_time.data(), 0777)) {
		return false;
	}

	SetLogDir(log_file_with_time.data());

	removeExtraLogs(); // Make sure we don't have more than ten logs per log type

	EA::Thread::SetAllocator(&_thread_allocator);

	if (!_log_mgr.init(log_file_with_time.data())) {
		return false;
	}

	LogInfoDefault("Initializing...");

	if (!_job_pool.init(static_cast<int32_t>(Gaff::GetNumberOfCores()), App::ThreadInit, App::ThreadShutdown)) {
		LogErrorDefault("Failed to initialize thread pool.");
		return false;
	}

	const Gaff::JSON read_file_threads = _configs.getObject(k_config_app_read_file_threads);
	_job_pool.addPool(HashStringView32<>(k_config_app_read_file_pool_name), read_file_threads.getInt32(k_config_app_default_read_file_threads));

	if (!loadFileSystem()) {
		return false;
	}

	_reflection_mgr.registerTypeBucket(CLASS_HASH(Shibboleth::IMainLoop));
	_reflection_mgr.registerTypeBucket<Refl::IAttribute>();
	_reflection_mgr.registerTypeBucket<IManager>();

	// Init engine reflection.
	for (int32_t mode_count = 0; mode_count < static_cast<int32_t>(InitMode::Count); ++mode_count) {
		Gen::Engine::InitReflection(static_cast<InitMode>(mode_count));
	}

	if (!loadModules()) {
		return false;
	}

	if (!loadMainLoop()) {
		return false;
	}

	LogInfoDefault("Game Successfully Initialized.");
	return true;
}

// Still single-threaded at this point, so ok that we're not using the spinlock
bool App::loadFileSystem(void)
{
	const Gaff::JSON& lfs = _configs.getObject(k_config_app_file_system);
	U8String fs = u8"";

	if (lfs.isString()) {
		fs.append_sprintf(u8"%s%s", lfs.getString(), TARGET_SUFFIX_U8 DYNAMIC_MODULE_EXTENSION_U8);

		_fs.file_system_module = _dynamic_loader.loadModule(fs.data(), u8"FileSystem");

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
	if (_configs.getObject(k_config_app_no_main_loop).isTrue()) {
		return true;
	}

	const Vector<const Refl::IReflectionDefinition*>* bucket = _reflection_mgr.getTypeBucket(CLASS_HASH(Shibboleth::IMainLoop));

	if (!bucket || bucket->empty()) {
		LogErrorDefault("Failed to find a class that implements the 'IMainLoop' interface.");
		return false;
	}

	const Refl::IReflectionDefinition* refl = bucket->front();

	if (const Gaff::JSON loop_name = _configs.getObject(k_config_app_main_loop); loop_name.isString()) {
		const char8_t* const loop_name_beg = loop_name.getString();
		const char8_t* const loop_name_end = loop_name_beg + loop_name.size();
		bool found = false;

		for (const Refl::IReflectionDefinition* ref_def : *bucket) {
			const char8_t* const name_beg = ref_def->getReflectionInstance().getName();
			const char8_t* const name_end = name_beg + eastl::CharStrlen(name_beg);

			if (!U8String::compare(name_beg, name_end, loop_name_beg, loop_name_end)) {
				refl = ref_def;
				found = true;
				break;
			}
		}

		if (!found) {
			LogErrorDefault("Failed to find main loop class '%s'.", loop_name_beg);
			loop_name.freeString(loop_name_beg);
			return false;
		}

		loop_name.freeString(loop_name_beg);
	}

	_main_loop = refl->template createT<IMainLoop>(CLASS_HASH(Shibboleth::IMainLoop), ProxyAllocator::GetGlobal());

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
	if (_configs.getObject(k_config_app_no_load_modules).isTrue()) {
		_job_pool.run();
		return true;
	}

#ifdef SHIB_STATIC
	if (!Gen::Engine::LoadModulesStatic()) {
		return false;
	}

#else
	const Gaff::JSON& module_dirs = _configs.getObject(k_config_module_directories);

	if (!module_dirs.isNull() && !module_dirs.isArray()) {
		LogErrorDefault("'module_directories' config option is not an array of strings.");
		return false;
	}

	if (module_dirs.isArray()) {
		const char8_t separator = Gaff::ConvertChar<char8_t>(std::filesystem::path::preferred_separator);

		for (int32_t i = 0; i < module_dirs.size(); ++i) {
			const Gaff::JSON dir = module_dirs.getObject(i);

			for (const auto& dir_entry : std::filesystem::directory_iterator(dir.getString())) {
				if (!dir_entry.is_regular_file()) {
					continue;
				}

				const auto abs_path = std::filesystem::absolute(dir_entry.path());
				const auto* name = abs_path.c_str();
				CONVERT_STRING(char8_t, temp, name);

				U8String module_name(temp);
				const size_t pos = module_name.rfind(separator);

				GAFF_ASSERT(pos != U8String::npos);

				module_name = module_name.substr(pos + 1, module_name.size() - eastl::CharStrlen(u8"Module" TARGET_SUFFIX_U8 DYNAMIC_MODULE_EXTENSION_U8) - (pos + 1));

				DynamicLoader::ModulePtr module = _dynamic_loader.loadModule(temp, module_name.data());

				if (!module) {
					LogWarningDefault("Failed to find or load dynamic module '%s'.", module_name.data());
					return false;

				#ifdef INIT_STACKTRACE_SYSTEM
					Gaff::StackTrace::RefreshModuleList(); // Will fix symbols from DLLs not resolving.
				#endif
				}

				CreateModuleFunc create_func = module->getFunc<CreateModuleFunc>("CreateModule");

				if (create_func) {
					if (!createModule(create_func, module_name.data())) {
						return false;
					}

				} else {
					LogErrorDefault("Failed to find function 'CreateModule' in dynamic module '%s'.", module_name.data());
				}
			}
		}

	} else {
		LogErrorDefault("Malformed configuration. '%s' is not an array of strings.", k_config_module_directories);
		return false;
	}
#endif

	_module_map.shrink_to_fit();

	for (auto& entry : _module_map) {
		if (!entry.second->preInit(*this)) {
			//LogErrorDefault("Module::preInit() failed for module '%s'.", entry.first.getBuffer());
			return false;
		}
	}

	for (auto& entry : _module_map) {
		entry.second->initReflectionEnums();
	}

	for (auto& entry : _module_map) {
		entry.second->initReflectionAttributes();
	}

	for (auto& entry : _module_map) {
		entry.second->initReflectionClasses();
	}

	for (auto& entry : _module_map) {
		entry.second->initNonOwned();
	}

	for (auto& entry : _module_map) {
		if (!entry.second->postInit()) {
			//LogErrorDefault("Module::preInit() failed for module '%s'.", entry.first.getBuffer());
			return false;
		}
	}

	const Gaff::JSON& module_load_order = _configs.getObject(k_config_module_load_order);
	const bool no_managers = _configs.getObject(k_config_app_no_managers).getBool(false);

	if (!module_load_order.isNull() && !module_load_order.isArray()) {
		LogErrorDefault("'module_load_order' config option is not an array.");
		return false;
	}

	// Create manager instances.
	if (!no_managers) {
		// Create managers from module load order first.
		if (module_load_order.isArray()) {
			const int32_t size = module_load_order.size();

			for (int32_t i = 0; i < size; ++i) {
				const Gaff::JSON& module_row = module_load_order.getObject(i);

				if (!module_row.isString()) {
					LogErrorDefault("module_load_order[%i] is not a string.", i);
					continue;
				}

				const char8_t* const module_name = module_row.getString();
				const Vector<const Refl::IReflectionDefinition*>* const manager_bucket =
					_reflection_mgr.getTypeBucket(Refl::Reflection<IManager>::GetNameHash(), Gaff::FNV1aHash64String(module_name));

				if (manager_bucket) {
					if (!createManagersInternal(*manager_bucket)) {
						// $TODO: Log error.
						return false;
					}
				}

				module_row.freeString(module_name);
			}
		}

		// Create the rest of the managers.
		const Vector<const Refl::IReflectionDefinition*>* manager_bucket = _reflection_mgr.getTypeBucket(Refl::Reflection<IManager>::GetNameHash());

		if (manager_bucket) {
			if (!createManagersInternal(*manager_bucket)) {
				// $TODO: Log error.
				return false;
			}
		}
	}

	_manager_map.shrink_to_fit();

	_job_pool.run();

	// Call initAllModulesLoaded() on all managers.
	Vector<const Refl::IReflectionDefinition*> already_initialized_managers;

	// Call on module load order first.
	if (module_load_order.isArray()) {
		const int32_t size = module_load_order.size();

		for (int32_t i = 0; i < size; ++i) {
			const Gaff::JSON& module_row = module_load_order.getObject(i);

			if (!module_row.isString()) {
				LogErrorDefault("module_load_order[%i] is not a string.", i);
				continue;
			}

			const char8_t* const module_name = module_row.getString();
			const Vector<const Refl::IReflectionDefinition*>* manager_bucket = _reflection_mgr.getTypeBucket(Refl::Reflection<IManager>::GetNameHash(), Gaff::FNV1aHash64String(module_name));

			if (manager_bucket) {
				for (const Refl::IReflectionDefinition* ref_def : *manager_bucket) {
					if (!_manager_map[ref_def->getReflectionInstance().getNameHash()]->initAllModulesLoaded()) {
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

	return true;
}

bool App::initApp(void)
{
	const Gaff::JSON& wd = _configs.getObject(k_config_app_working_dir);

	if (wd.isString()) {
		const char8_t* const working_dir = wd.getString();

		if (!Gaff::SetWorkingDir(working_dir)) {
			LogErrorDefault("Failed to set working directory to '%s'.", working_dir);
			return false;
		}

		// Set DLL auto-load directory.
#ifdef PLATFORM_WINDOWS
		CONVERT_STRING(wchar_t, temp, working_dir);

		if ((temp[wd.size() - 1] == L'/' || temp[wd.size() - 1] == L'\\')) {
			memcpy(temp + wd.size(), L"bin", sizeof(wchar_t) * 4);
		} else {
			memcpy(temp + wd.size(), L"/bin", sizeof(wchar_t) * 4);
		}

		if (!SetDllDirectory(temp)) {
			LogErrorDefault("Failed to set DLL directory to '%ls'.", temp);
			return false;
		}
#endif

		wd.freeString(working_dir);

#ifdef PLATFORM_WINDOWS
	} else {
		if (!SetDllDirectory(L"bin")) {
			LogErrorDefault("Failed to set DLL directory to 'bin'.");
			return false;
		}
#endif
	}

	return true;
}

void App::removeExtraLogs(void)
{
	const Gaff::JSON log_dir_holder = _configs.getObject(k_config_app_log_dir); // Need to hold this otherwise the string gets deallocated.
	const char8_t* const log_dir = log_dir_holder.getString(k_config_app_default_log_dir);
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

bool App::createManagersInternal(const Vector<const Refl::IReflectionDefinition*>& managers)
{
	ProxyAllocator allocator;

	for (const Refl::IReflectionDefinition* ref_def : managers) {
		if (hasManager(ref_def->getReflectionInstance().getNameHash())) {
			continue;
		}

		IManager* const manager = ref_def->createT<IManager>(allocator);

		if (!manager->init()) {
			LogErrorDefault("Failed to initialize manager '%s'!", ref_def->getReflectionInstance().getName());
			SHIB_FREET(manager, GetAllocator());
			return false;
		}

		const EA::Thread::ThreadId thread_id = EA::Thread::GetThreadId();
		const uintptr_t id_int = (uintptr_t)&thread_id;

		if (!manager->initThread(id_int)) {
			LogErrorDefault("Failed to initialize manager '%s' for main thread!", ref_def->getReflectionInstance().getName());
			SHIB_FREET(manager, GetAllocator());
			return false;
		}

		const Gaff::Hash64 name = ref_def->getReflectionInstance().getNameHash();

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

bool App::createModule(CreateModuleFunc create_func, const char8_t* module_name)
{
	auto module_ptr_it = _module_map.insert(Gaff::FNV1aHash64String(module_name)).first;
	module_ptr_it->second.reset(create_func());

	if (!module_ptr_it->second) {
		LogWarningDefault("Failed to allocate IModule for module '%s'.", module_name);
		return false;
	}

	return true;
}

void App::ModuleChanged(const char8_t* path)
{
	if (!Gaff::EndsWith(path, TARGET_SUFFIX_U8 DYNAMIC_MODULE_EXTENSION_U8)) {
		return;
	}

	static_cast<App&>(GetApp())._dynamic_loader.forEachModule([&](const HashString32<>& name, const DynamicLoader::ModulePtr& module_ptr) -> bool
	{
		if (Gaff::Find(path, name.getBuffer()) == GAFF_SIZE_T_FAIL) {
			return false;
		}

		// Copy module to workingdir.
		// Load new module and replace old data structures with new ones.
		// Unload old module and delete file.

		GAFF_REF(module_ptr);

		return false;
	});
}

void App::ThreadInit(uintptr_t thread_id)
{
	App& app = static_cast<App&>(GetApp());

	for (const auto& entry : app._manager_map) {
		if (!entry.second->initThread(thread_id)) {
			LogErrorDefault("Failed to initialize thread for '%s'.", entry.second->getReflectionDefinition().getFriendlyName());

			app.quit();
			break;
		}
	}
}

void App::ThreadShutdown(uintptr_t thread_id)
{
	App& app = static_cast<App&>(GetApp());

	for (const auto& entry : app._manager_map) {
		entry.second->destroyThread(thread_id);
	}
}

NS_END
