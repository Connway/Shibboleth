/************************************************************************************
Copyright (C) 2018 by Nicholas LaCroix

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
#include "Gaff_ReflectionInterfaces.h"
#include <Gaff_CrashHandler.h>
#include <Gaff_Directory.h>
#include <Gaff_Utils.h>
#include <Gaff_JSON.h>
#include <Gaff_File.h>
#include <regex>

#ifdef PLATFORM_WINDOWS
	#include <shellapi.h>
#endif

#ifdef INIT_STACKTRACE_SYSTEM
	#include <Gaff_StackTrace.h>
#endif

NS_SHIBBOLETH

App::App(void):
	_running(true), _main_loop(nullptr)
{
	Gaff::InitializeCrashHandler();
	// Set crash handler here

	SetApp(*this);
}

App::~App(void)
{
}

// Still single-threaded at this point, so ok that we're not locking.
bool App::init(int argc, const char** argv)
{
	// If we can't load the initial config, then create an empty object.
	if (!_configs.parseFile("cfg/app.cfg")) {
		_configs = Gaff::JSON::CreateObject();
	}

	// Load any extra configs and add their values to the _configs object.
	for (int i = 0; i < argc; ++i) {
		const char* const arg = argv[i];
		Gaff::JSON extra_configs;

		if (!Gaff::File::CheckExtension(arg, ".cfg") || !extra_configs.parseFile(arg)) {
			continue;
		}

		extra_configs.forEachInObject([&](const char* key, const Gaff::JSON& value) -> bool {
			_configs[key] = value;
			return false;
		});
	}

	return initInternal();
}

#ifdef PLATFORM_WINDOWS
// Still single-threaded at this point, so ok that we're not locking.
bool App::init(void)
{
	// If we can't load the initial config, then create an empty object.
	if (!_configs.parseFile("cfg/app.cfg")) {
		_configs = Gaff::JSON::CreateObject();
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
			_configs[key] = value;
			return false;
		});
	}

	return initInternal();
}
#endif

bool App::initInternal(void)
{
#ifdef INIT_STACKTRACE_SYSTEM
	GAFF_ASSERT(Gaff::StackTrace::Init());
#endif

	if (!initApp()) {
		return false;
	}

	if (!_log_mgr.init()) {
		return false;
	}

	removeExtraLogs(); // Make sure we don't have more than ten logs per log type

	if (!Gaff::CreateDir("./logs", 0777)) {
		return false;
	}

	LogInfoDefault(
		"==================================================\n"
		"==================================================\n"
		"Initializing Game...\n"
	);

	auto thread_init = []()
	{
		AllocatorThreadInit();
	};

	if (!_job_pool.init(JPI_SIZE - 1, static_cast<int32_t>(Gaff::GetNumberOfCores()), thread_init)) {
		LogErrorDefault("ERROR - Failed to initialize thread pool\n");
		return false;
	}

	if (!loadFileSystem()) {
		return false;
	}

#ifdef INIT_STACKTRACE_SYSTEM
	Gaff::StackTrace::RefreshModuleList(); // Will fix symbols from DLLs not resolving.
#endif

	_reflection_mgr.registerTypeBucket(Gaff::FNV1aHash64Const("Gaff::IAttribute"));
	_reflection_mgr.registerTypeBucket(Gaff::FNV1aHash64Const("IManager"));
	_reflection_mgr.registerTypeBucket(Gaff::FNV1aHash64Const("**")); // All types not registered with a type bucket.
	_reflection_mgr.registerTypeBucket(Gaff::FNV1aHash64Const("*"));  // All reflection.

	if (!loadModules()) {
		return false;
	}

#ifdef INIT_STACKTRACE_SYSTEM
	Gaff::StackTrace::RefreshModuleList(); // Will fix symbols from DLLs not resolving.
#endif

	if (!loadMainLoop()) {
		return false;
	}

#ifdef INIT_STACKTRACE_SYSTEM
	Gaff::StackTrace::RefreshModuleList(); // Will fix symbols from DLLs not resolving.
#endif

	LogInfoDefault("Game Successfully Initialized\n\n");
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

		_fs.file_system_module = _dynamic_loader.loadModule(fs.data(), "File System");

		if (!_fs.file_system_module) {
			LogInfoDefault("Failed to find filesystem '%s'.\n", fs.data());
			return false;
		}

		LogInfoDefault("Found '%s'. Creating file system.\n", fs.data());

		InitModuleFunc init_func = _fs.file_system_module->getFunc<InitModuleFunc>("InitModule");

		if (init_func && !init_func(*this)) {
			LogErrorDefault("ERROR - Failed to init file system module.\n");
			return false;
		}

		_fs.destroy_func = _fs.file_system_module->getFunc<FileSystemData::DestroyFileSystemFunc>("DestroyFileSystem");
		_fs.create_func = _fs.file_system_module->getFunc<FileSystemData::CreateFileSystemFunc>("CreateFileSystem");

		if (!_fs.create_func) {
			LogErrorDefault("ERROR - Failed to find 'CreateFileSystem' in '%s'.\n", fs.data());
			return false;
		}

		if (!_fs.destroy_func) {
			LogErrorDefault("ERROR - Failed to find 'DestroyFileSystem' in '%s'.\n", fs.data());
			return false;
		}

		_fs.file_system = _fs.create_func();

		if (!_fs.file_system) {
			LogErrorDefault("ERROR - Failed to create file system from '%s'.\n", fs.data());
			return false;
		}

	} else {
		LogInfoDefault("Defaulting to loose file system.\n", fs.data());
		_fs.file_system = SHIB_ALLOCT(LooseFileSystem, *GetAllocator());

		if (!_fs.file_system) {
			LogErrorDefault("ERROR - Failed to create loose file system.\n");
			return false;
		}
	}

	return true;
}

bool App::loadMainLoop(void)
{
	const char* main_loop_module = "MainLoop" BIT_EXTENSION DYNAMIC_EXTENSION;
	DynamicLoader::ModulePtr module = _dynamic_loader.loadModule(main_loop_module, "MainLoop");

	if (module) {
		InitModuleFunc init_func = module->getFunc<InitModuleFunc>("InitModule");

		if (!init_func) {
			LogErrorDefault("ERROR - Failed to find function 'InitModule' in dynamic module '%s'\n", main_loop_module);
			return false;
		}

		_main_loop = module->getFunc<MainLoopFunc>("MainLoop");

		if (!_main_loop) {
			LogErrorDefault("ERROR - Failed to find function 'MainLoop' in dynamic module '%s'\n", main_loop_module);
			return false;
		}

		if (!init_func(*this)) {
			LogErrorDefault("ERROR - Failed to initialize '%s'\n", main_loop_module);
			return false;
		}

	} else {
		LogErrorDefault("ERROR - Failed to open module '%s'\n", main_loop_module);
		return false;
	}

	return true;
}

bool App::loadModules(void)
{
	// Load module order list.
	Gaff::JSON modules_cfg;

	if (modules_cfg.parseFile("cfg/module_load_order.cfg")) {
		GAFF_ASSERT(modules_cfg.isArray());

		// Load those modules first.
		modules_cfg.forEachInArray([&](int32_t, const Gaff::JSON& value) -> bool
		{
			GAFF_ASSERT(value.isString());
			const U8String path = U8String(value.getString()) + "Module" BIT_EXTENSION DYNAMIC_EXTENSION;
			loadModule(path.data());
			return false;
		});

	} else {
		LogWarningDefault("Failed to load 'manual_load_order.cfg'!");
	}

	// Then load the rest.
	const bool error = Gaff::ForEachTypeInDirectory<Gaff::FDT_RegularFile>("./Modules", [&](const char* name, size_t) -> bool
	{
		return !(_dynamic_loader.getModule(name) || loadModule(name));
	});

	if (error) {
		return false;
	}

	// Create manager instances.
	const Vector<const Gaff::IReflectionDefinition*>* manager_bucket = _reflection_mgr.getTypeBucket(Gaff::FNV1aHash64Const("IManager"));

	for (const Gaff::IReflectionDefinition* ref_def : *manager_bucket) {

		ProxyAllocator allocator;
		IManager* manager = ref_def->CREATEALLOCT(IManager, allocator);

		if (!manager->init()) {
			// log error
			SHIB_FREET(manager, *GetAllocator());
			return false;
		}

		const Gaff::Hash64 name = ref_def->getReflectionInstance().getHash();

		GAFF_ASSERT(_manager_map.find(name) == _manager_map.end());
		_manager_map[name].reset(manager);
	}

	// Notify all modules that every module has been loaded.
	_dynamic_loader.forEachModule([&](const HashString32&, DynamicLoader::ModulePtr& module) -> bool
	{
		using AllModulesLoadedFunc = void (*)(void);
		AllModulesLoadedFunc aml_func = module->getFunc<AllModulesLoadedFunc>("AllModulesLoaded");

		if (aml_func) {
			aml_func();
		}

		return false;
	});

	// Notify all managers that every module has been loaded.
	for (auto it = _manager_map.begin(); it != _manager_map.end(); ++it) {
		it->second->allModulesLoaded();
	}

	return true;
}

bool App::initApp(void)
{
	const Gaff::JSON& wd = _configs["working_dir"];

	if (wd.isString()) {
		if (!Gaff::SetWorkingDir(wd.getString())) {
			LogErrorDefault("ERROR - Failed to set working directory to '%s'.\n", wd.getString());
			return false;
		}

		// Set DLL auto-load directory.
#ifdef PLATFORM_WINDOWS
		const char8_t* working_dir_ptr = wd.getString();
		CONVERT_STRING(wchar_t, temp, working_dir_ptr);

		if ((temp[wd.size() - 1] == TEXT('/') || temp[wd.size() - 1] == TEXT('\\'))) {
			memcpy(temp + wd.size(), TEXT("bin"), sizeof(wchar_t));
		} else {
			memcpy(temp + wd.size(), TEXT("/bin"), sizeof(wchar_t));
		}

		if (!SetDllDirectory(temp)) {
			LogErrorDefault("ERROR - Failed to set working directory to '%s'.\n", wd.getString());
			return false;
		}

	} else {
		if (!SetDllDirectory(TEXT("bin"))) {
			LogErrorDefault("ERROR - Failed to set working directory to '%s'.\n", wd.getString());
			return false;
		}
#endif
	}

	return true;
}

bool App::loadModule(const char* module_name)
{
	// If it's not a dynamic module,
	// or if it is not compiled for our architecture and build mode,
	// then just skip over it.
	if (!Gaff::File::CheckExtension(module_name, BIT_EXTENSION DYNAMIC_EXTENSION)) {
		return true;
	}

	U8String rel_path = U8String("./Modules/") + module_name;

#ifdef PLATFORM_WINDOWS
	DynamicLoader::ModulePtr module = _dynamic_loader.loadModule(("../" + rel_path).data(), module_name);
#else
	DynamicLoader::ModulePtr module = _dynamic_loader.loadModule(rel_path.data(), name);
#endif

	if (!module) {
		LogWarningDefault("Failed to find or load dynamic module '%s'\n", rel_path.data());
		return false;
	}

	InitModuleFunc init_func = module->getFunc<InitModuleFunc>("InitModule");

	if (!init_func) {
		LogErrorDefault("Failed to find function 'InitModule' in dynamic module '%s'\n", rel_path.data());
		return false;
	}
	else if (!init_func(*this)) {
		LogErrorDefault("Failed to initialize dynamic module '%s'\n", rel_path.data());
		return false;
	}

	return true;
}

void App::removeExtraLogs(void)
{
	int32_t callstack_log_count = 0;
	int32_t alloc_log_count = 0;
	int32_t game_log_count = 0;
	int32_t leak_log_count = 0;

	Gaff::ForEachTypeInDirectory<Gaff::FDT_RegularFile>("./Logs", [&](const char* name, size_t) -> bool
	{
		if (std::regex_match(name, std::regex("Log.+\.txt"))) {
			++game_log_count;
		} else if (std::regex_match(name, std::regex("AllocationLog.+\.txt"))) {
			++alloc_log_count;
		} else if (std::regex_match(name, std::regex("CallstackLog.+\.txt"))) {
			++callstack_log_count;
		} else if (std::regex_match(name, std::regex("LeakLog.+\.txt"))) {
			++leak_log_count;
		}

		return false;
	});

	int32_t callstack_logs_delete = (callstack_log_count > 10) ? callstack_log_count - 10 : 0;
	int32_t alloc_logs_delete = (alloc_log_count > 10) ? alloc_log_count - 10 : 0;
	int32_t game_logs_delete = (game_log_count > 10) ? game_log_count - 10 : 0;
	int32_t leak_logs_delete = (leak_log_count > 10) ? leak_log_count - 10 : 0;
	callstack_log_count = alloc_log_count = game_log_count = leak_log_count = 0;
	U8String temp;

	Gaff::ForEachTypeInDirectory<Gaff::FDT_RegularFile>("./logs", [&](const char* name, size_t) -> bool
	{
		if (std::regex_match(name, std::regex("GameLog.+\.txt")) && game_log_count < game_logs_delete) {
			temp = U8String("./logs/") + name;
			std::remove(temp.data());
			++game_log_count;

		} else if (std::regex_match(name, std::regex("AllocationLog.+\.txt")) && alloc_log_count < alloc_logs_delete) {
			temp = U8String("./logs/") + name;
			std::remove(temp.data());
			++alloc_log_count;

		} else if (std::regex_match(name, std::regex("CallstackLog.+\.txt")) && callstack_log_count < callstack_logs_delete) {
			temp = U8String("./logs/") + name;
			std::remove(temp.data());
			++callstack_log_count;

		} else if (std::regex_match(name, std::regex("LeakLog.+\.txt")) && leak_log_count < leak_logs_delete) {
			temp = U8String("./logs/") + name;
			std::remove(temp.data());
			++leak_log_count;
		}

		return false;
	});
}

void App::run(void)
{
	while (_running) {
		//uv_run(uv_default_loop(), UV_RUN_NOWAIT);
		//_broadcaster.update();
		_main_loop();
	}
}

void App::destroy(void)
{
	_job_pool.destroy();

	_reflection_mgr.destroy();
	_manager_map.clear();
	_log_mgr.destroy();

	_dynamic_loader.forEachModule([](const HashString32&, DynamicLoader::ModulePtr module) -> bool
	{
		void (*shutdown_func)(void) = module->getFunc<void (*)(void)>("ShutdownModule");

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
		SHIB_FREET(_fs.file_system, *GetAllocator());
	}

#ifdef INIT_STACKTRACE_SYSTEM
	Gaff::StackTrace::Destroy();
#endif
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

IFileSystem* App::getFileSystem(void)
{
	return _fs.file_system;
}

const Gaff::JSON& App::getConfigs(void) const
{
	return _configs;
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

DynamicLoader::ModulePtr App::loadModule(const char* filename, const char* name)
{
	return _dynamic_loader.loadModule(filename, name);
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
