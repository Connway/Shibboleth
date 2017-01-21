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

// Still single-threaded at this point, so ok that we're not using the spinlock
bool App::init(int argc, char** argv)
{
#ifdef INIT_STACKTRACE_SYSTEM
	GAFF_ASSERT(Gaff::StackTrace::Init());
#endif

	_cmd_line_args = Gaff::ParseCommandLine<ProxyAllocator>(argc, argv);

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

	if (!_job_pool.init()) {
		LogErrorDefault("ERROR - Failed to initialize thread pool\n");
		return false;
	}

	if (!loadFileSystem()) {
		return false;
	}

#ifdef INIT_STACKTRACE_SYSTEM
	Gaff::StackTrace::RefreshModuleList(); // Will fix symbols from DLLs not resolving.
#endif

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
	if (Gaff::Find(_cmd_line_args, Gaff::FNV1aHash32String("lfs")) != _cmd_line_args.end() ||
		Gaff::Find(_cmd_line_args, Gaff::FNV1aHash32String("loose_file_system")) != _cmd_line_args.end()) {

		_fs.file_system_module = _dynamic_loader.loadModule("./FileSystem" BIT_EXTENSION DYNAMIC_EXTENSION, "File System");
	}

	if (_fs.file_system_module) {
		LogInfoDefault("Found 'FileSystem" BIT_EXTENSION DYNAMIC_EXTENSION "'. Creating file system\n");

		InitModuleFunc init_func = _fs.file_system_module->getFunc<InitModuleFunc>("InitModule");

		if (init_func && !init_func(*this)) {
			LogErrorDefault("ERROR - Failed to init file system module\n");
			return false;
		}

		_fs.destroy_func = _fs.file_system_module->getFunc<FileSystemData::DestroyFileSystemFunc>("DestroyFileSystem");
		_fs.create_func = _fs.file_system_module->getFunc<FileSystemData::CreateFileSystemFunc>("CreateFileSystem");

		if (!_fs.create_func) {
			LogErrorDefault("ERROR - Failed to find 'CreateFileSystem' in 'FileSystem" BIT_EXTENSION DYNAMIC_EXTENSION "'\n");
			return false;
		}

		if (!_fs.destroy_func) {
			LogErrorDefault("ERROR - Failed to find 'DestroyFileSystem' in 'FileSystem" BIT_EXTENSION DYNAMIC_EXTENSION "'\n");
			return false;
		}

		_fs.file_system = _fs.create_func();

		if (!_fs.file_system) {
			LogErrorDefault("ERROR - Failed to create file system from 'FileSystem" BIT_EXTENSION DYNAMIC_EXTENSION "'\n");
			return false;
		}

	} else {
		LogInfoDefault("Could not find 'FileSystem" BIT_EXTENSION DYNAMIC_EXTENSION "' defaulting to loose file system\n");
		_fs.file_system = SHIB_ALLOCT(LooseFileSystem, *GetAllocator());

		if (!_fs.file_system) {
			LogErrorDefault("ERROR - Failed to create loose file system\n");
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
	// load module order list
	// load those modules first
	// then load the rest

	Gaff::JSON modules_cfg;

	if (modules_cfg.parseFile("cfg/module_load_order.cfg")) {
		GAFF_ASSERT(modules_cfg.isArray());

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

	const bool error = Gaff::ForEachTypeInDirectory<Gaff::FDT_RegularFile>("./Modules", [&](const char* name, size_t) -> bool
	{
		return !(_dynamic_loader.getModule(name) || loadModule(name));
	});

	if (!error) {
		_dynamic_loader.forEachModule([&](DynamicLoader::ModulePtr& module) -> bool
		{
			using AllModulesLoadedFunc = void (*)(void);
			AllModulesLoadedFunc aml_func = module->getFunc<AllModulesLoadedFunc>("AllModulesLoaded");

			if (aml_func) {
				aml_func();
			}

			return false;
		});

		for (auto it = _manager_map.begin(); it != _manager_map.end(); ++it) {
			it->second->allModulesLoaded();
		}
	}

	return !error;
}

bool App::initApp(void)
{
	auto it = Gaff::Find(_cmd_line_args, Gaff::HashStringTemp32("working_dir"));

	if (it != _cmd_line_args.end()) {
		const U8String& working_dir = it->second;

		if (!Gaff::SetWorkingDir(working_dir.data())) {
			LogErrorDefault("ERROR - Failed to set working directory to '%s'.\n", working_dir.data());
			return false;
		}

	// Set DLL auto-load directory.
#ifdef PLATFORM_WINDOWS
		const char8_t* working_dir_ptr = working_dir.data();
		CONVERT_STRING(wchar_t, temp, working_dir_ptr);

		if ((temp[working_dir.size() - 1] == TEXT('/') || temp[working_dir.size() - 1] == TEXT('\\'))) {
			memcpy(temp + working_dir.size(), TEXT("bin"), sizeof(TCHAR));
		} else {
			memcpy(temp + working_dir.size(), TEXT("/bin"), sizeof(TCHAR));
		}

		SetDllDirectory(temp);

	} else {
		SetDllDirectory(TEXT("bin"));
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
	unsigned int callstack_log_count = 0;
	unsigned int alloc_log_count = 0;
	unsigned int game_log_count = 0;
	unsigned int leak_log_count = 0;

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

	unsigned int callstack_logs_delete = (callstack_log_count > 10) ? callstack_log_count - 10 : 0;
	unsigned int alloc_logs_delete = (alloc_log_count > 10) ? alloc_log_count - 10 : 0;
	unsigned int game_logs_delete = (game_log_count > 10) ? game_log_count - 10 : 0;
	unsigned int leak_logs_delete = (leak_log_count > 10) ? leak_log_count - 10 : 0;
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
		_broadcaster.update();
		_main_loop();
	}
}

void App::destroy(void)
{
	_job_pool.destroy();

	_reflection_map.clear();
	_manager_map.clear();
	_log_mgr.destroy();

	_dynamic_loader.forEachModule([](DynamicLoader::ModulePtr module) -> bool
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

MessageBroadcaster& App::getBroadcaster(void)
{
	return _broadcaster;
}

IFileSystem* App::getFileSystem(void)
{
	return _fs.file_system;
}

const VectorMap<HashString32, U8String>& App::getCmdLine(void) const
{
	return _cmd_line_args;
}

VectorMap<HashString32, U8String>& App::getCmdLine(void)
{
	return _cmd_line_args;
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

const Gaff::IReflectionDefinition* App::getReflection(Gaff::Hash64 name) const
{
	auto it = _reflection_map.find(name);
	return (it == _reflection_map.end()) ? nullptr : it->second.get();
}

void App::registerReflection(Gaff::Hash64 name, Gaff::IReflectionDefinition& ref_def)
{
	GAFF_ASSERT(_reflection_map.find(name) == _reflection_map.end());
	_reflection_map[name].reset(&ref_def);

	// Register if a manager.
	if (ref_def.hasInterface(Gaff::FNV1aHash64Const("IManager"))) {
		ProxyAllocator allocator;
		IManager* manager = ref_def.createAllocT<IManager>(allocator);

		if (!manager->init()) {
			// log error
			SHIB_FREET(manager, *GetAllocator());
			return;
		}

		GAFF_ASSERT(_manager_map.find(name) == _manager_map.end());
		_manager_map[name].reset(manager);
	}

	// Check if this type implements an interface that has a type bucket request.
	for (auto it = _type_buckets.begin(); it != _type_buckets.end(); ++it) {
		if (ref_def.hasInterface(it->first)) {
			it->second.emplace_back(name);
		}
	}
}

void App::registerTypeBucket(Gaff::Hash64 name)
{
	if (_type_buckets.find(name) != _type_buckets.end()) {
		return;
	}

	Vector<Gaff::Hash64>& types = _type_buckets[name];

	for (const auto& ref_map_pair : _reflection_map) {
		const auto& ref_def = ref_map_pair.second;
		const Gaff::Hash64 class_hash = ref_def->getReflectionInstance().getHash();

		// Don't care about the concrete version of class. Only reflected types that inherit from the class.
		if (name != class_hash && ref_def->hasInterface(name)) {
			types.emplace_back(class_hash);
		}
	}
}

const Vector<Gaff::Hash64>* App::getTypeBucket(Gaff::Hash64 name) const
{
	auto it = _type_buckets.find(name);
	return (it == _type_buckets.end()) ? nullptr : &it->second;
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
