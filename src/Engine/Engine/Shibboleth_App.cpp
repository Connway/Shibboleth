/************************************************************************************
Copyright (C) 2020 by Nicholas LaCroix

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
#include "Shibboleth_Utilities.h"
#include "Shibboleth_IMainLoop.h"
#include "Shibboleth_IManager.h"
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
bool App::init(int argc, const char** argv, bool (*static_init)(void))
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

	return initInternal(static_init);
}

#ifdef PLATFORM_WINDOWS
// Still single-threaded at this point, so ok that we're not locking.
bool App::init(bool (*static_init)(void))
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

	return initInternal(static_init);
}
#endif

void App::run(void)
{
	while (_running) {
		//uv_run(uv_default_loop(), UV_RUN_NOWAIT);
		_main_loop->update();
	}
}

void App::destroy(void (*static_shutdown)(void))
{
	if (_main_loop) {
		_main_loop->destroy();
		SHIB_FREET(_main_loop, GetAllocator());
		_main_loop = nullptr;
	}

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
				const auto* const manager_refls = _reflection_mgr.getTypeBucket(CLASS_HASH(IManager), Gaff::FNV1aHash64String(module_name));

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

	if (static_shutdown) {
		static_shutdown();
	}

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
	auto it = _manager_map.find(name);
	GAFF_ASSERT(name && it != _manager_map.end());
	return it->second.get();
}

IManager* App::getManager(Gaff::Hash64 name)
{
	auto it = _manager_map.find(name);
	GAFF_ASSERT(name && it != _manager_map.end());
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

bool App::initInternal(bool (*static_init)(void))
{
	const char* const log_dir = _configs["log_dir"].getString("./logs");

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

	auto thread_init = []()
	{
		AllocatorThreadInit();
	};

	if (!_job_pool.init(JPI_SIZE - 1, static_cast<int32_t>(Gaff::GetNumberOfCores()), thread_init)) {
		LogErrorDefault("Failed to initialize thread pool.");
		return false;
	}

	_broadcaster.init();

	if (!loadFileSystem()) {
		return false;
	}

	_reflection_mgr.registerTypeBucket(Gaff::FNV1aHash64Const("Gaff::IAttribute"));
	_reflection_mgr.registerTypeBucket(Gaff::FNV1aHash64Const("IMainLoop"));
	_reflection_mgr.registerTypeBucket(Gaff::FNV1aHash64Const("IManager"));

	if (static_init && !static_init()) {
		return false;
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
	const Gaff::JSON& lfs = _configs["file_system"];
	U8String fs = "";

	if (lfs.isString()) {
		fs = U8String(lfs.getString()) + BIT_EXTENSION;
		fs += DYNAMIC_EXTENSION;

		_fs.file_system_module = _dynamic_loader.loadModule(fs.data(), "FileSystem");

		if (!_fs.file_system_module) {
			LogInfoDefault("Failed to find file system '%s'.", fs.data());
			return false;
		}

#ifdef INIT_STACKTRACE_SYSTEM
		Gaff::StackTrace::RefreshModuleList(); // Will fix symbols from DLLs not resolving.
#endif

		LogInfoDefault("Found '%s'. Creating file system.", fs.data());

		InitModuleFunc init_func = _fs.file_system_module->getFunc<InitModuleFunc>("InitModule");

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

	const bool no_managers = _configs["no_managers"].getBool(false);

	if (module_load_order.isArray()) {
		const int32_t size = module_load_order.size();

		// First pass, load specified modules first.
		for (int32_t i = 0; i < size; ++i) {
			const Gaff::JSON& module_row = module_load_order[i];

			if (module_row.isString()) {
				const char* const module_name = module_row.getString();
				const U8String module_path(U8String::CtorSprintf(), "%sModule" BIT_EXTENSION DYNAMIC_EXTENSION, module_name);

				if (!loadModule(module_path.data())) {
					LogErrorDefault("Failed to load module '%s'.", module_path.data());
					return false;
				}

				eastl::string_view name_view(module_name);
				const size_t delimeter = name_view.find_last_of('/');

				if (delimeter != eastl::string_view::npos) {
					name_view = name_view.substr(delimeter + 1);
				}

				const Vector<const Gaff::IReflectionDefinition*>* manager_bucket = _reflection_mgr.getTypeBucket(CLASS_HASH(IManager), Gaff::FNV1aHash64(name_view.data(), name_view.size()));

				if (manager_bucket) {
					if (!createManagersInternal(*manager_bucket)) {
						return false;
					}
				}

				module_row.freeString(module_name);

			} else if (module_row.isArray()) {
				if (!module_row[0].isString()) {
					LogErrorDefault("module_load_order[%i][0] is not a string.", i);
					return false;
				}

				if (!module_row[1].isArray()) {
					LogErrorDefault("module_load_order[%i][1] is not an array.", i);
					return false;
				}

				const char* const real_module_name = module_row[0].getString();
				const Gaff::Hash64 module_hash = Gaff::FNV1aHash64String(real_module_name);
				module_row[0].freeString(real_module_name);

				const int32_t row_size = module_row[1].size();
				bool success = false;

				for (int32_t j = 0; j < row_size; ++j) {
					const Gaff::JSON& module_row_element = module_row[1][j];

					if (!module_row_element.isString()) {
						LogErrorDefault("module_load_order[%i][1][%i] is not a string.", i, j);
					}

					const char* const module_name = module_row_element.getString();
					const U8String module_path(U8String::CtorSprintf(), "%sModule" BIT_EXTENSION DYNAMIC_EXTENSION, module_name);

					success = loadModule(module_path.data());

					if (success) {
						const Vector<const Gaff::IReflectionDefinition*>* manager_bucket = _reflection_mgr.getTypeBucket(CLASS_HASH(IManager), module_hash);

						if (manager_bucket) {
							if (!createManagersInternal(*manager_bucket)) {
								module_row_element.freeString(module_name);
								return false;
							}
						}

						module_row_element.freeString(module_name);
						break;
					}
				}

				if (!success) {
					LogErrorDefault("Failed to load any modules in module_load_order[%i].", i);
					return false;
				}

			} else {
				LogErrorDefault("module_load_order[%i] is not a string or an array of strings.", i);
			}
		}
	}

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

				if (!loadModule(temp)) {
					return false;
				}
			}
		}
	}

	// Create manager instances.
	if (!no_managers) {
		const Vector<const Gaff::IReflectionDefinition*>* manager_bucket = _reflection_mgr.getTypeBucket(CLASS_HASH(IManager));

		if (manager_bucket) {
			if (!createManagersInternal(*manager_bucket)) {
				return false;
			}
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

	// Notify all managers that every module has been loaded.
	for (auto& mgr_pair : _manager_map) {
		mgr_pair.second->allModulesLoaded();
	}

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

		if ((temp[wd.size() - 1] == TEXT('/') || temp[wd.size() - 1] == TEXT('\\'))) {
			memcpy(temp + wd.size(), TEXT("bin"), sizeof(wchar_t) * 4);
		} else {
			memcpy(temp + wd.size(), TEXT("/bin"), sizeof(wchar_t) * 4);
		}

		if (!SetDllDirectory(temp)) {
			LogErrorDefault("Failed to set DLL directory to '%s'.", wd.getString());
			return false;
		}

	} else {
		if (!SetDllDirectory(TEXT("bin"))) {
			LogErrorDefault("Failed to set DLL directory to '%s'.", wd.getString());
			return false;
		}
#endif
	}

	return true;
}

bool App::loadModule(const char* module_path)
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

	if (module) {
		return true;
	}

	module = _dynamic_loader.loadModule(temp, module_name.data());

	if (!module) {
		LogWarningDefault("Failed to find or load dynamic module '%s'.", module_name.data());
		return false;
	}

#ifdef INIT_STACKTRACE_SYSTEM
	Gaff::StackTrace::RefreshModuleList(); // Will fix symbols from DLLs not resolving.
#endif

	InitModuleFunc init_func = module->getFunc<InitModuleFunc>("InitModule");

	if (!init_func) {
		LogErrorDefault("Failed to find function 'InitModule' in dynamic module '%s'.", module_name.data());
		return false;

	} else if (!init_func(*this)) {
		LogErrorDefault("Failed to initialize dynamic module '%s'.", module_name.data());
		return false;
	}

	return true;
}

void App::removeExtraLogs(void)
{
	const char* const log_dir = _configs["log_dir"].getString("./logs");
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

		IManager* manager = ref_def->CREATET(IManager, allocator);

		if (!manager->init()) {
			// $TODO: Log error
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

NS_END
