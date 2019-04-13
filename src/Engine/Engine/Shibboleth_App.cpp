/************************************************************************************
Copyright (C) 2019 by Nicholas LaCroix

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
	// If we can't load the initial config, then create an empty object.
	if (!_configs.parseFile("cfg/app.cfg")) {
		_configs = Gaff::JSON::CreateObject();
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

		} else if (std::filesystem::is_directory(arg)) {
			_project_dir = arg;
		}
	}

	return initInternal(static_init);
}

#ifdef PLATFORM_WINDOWS
// Still single-threaded at this point, so ok that we're not locking.
bool App::init(bool (*static_init)(void))
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

	return initInternal(static_init);
}
#endif

bool App::initInternal(bool (*static_init)(void))
{
	if (!initApp()) {
		return false;
	}

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

		_fs.file_system_module = _dynamic_loader.loadModule(fs.data(), "File System");

		if (!_fs.file_system_module) {
			LogInfoDefault("Failed to find filesystem '%s'.", fs.data());
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
	const Vector<const Gaff::IReflectionDefinition*>* bucket = _reflection_mgr.getTypeBucket(Gaff::FNV1aHash64Const("IMainLoop"));

	if (!bucket || bucket->empty()) {
		LogErrorDefault("Failed to find a class that implements the 'IMainLoop' interface.");
		return false;
	}

	const Gaff::IReflectionDefinition* refl = bucket->front();

	if (_configs["editor_mode"].getBool(false)) {
		for (const Gaff::IReflectionDefinition* ref_def : *bucket) {
			if (ref_def->getClassAttr<EditorAttribute>()) {
				refl = ref_def;
				break;
			}
		}
	}

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
		LogErrorDefault("'module_directories' config option is not an array of strings!");
		return false;
	}

	if (!module_load_order.isNull() && !module_load_order.isArray()) {
		LogErrorDefault("'module_load_order' config option is not an array of strings!");
		return false;
	}

	if (module_load_order.isArray()) {
		const int32_t size = module_load_order.size();

		// First pass, load specified modules first.
		for (int32_t i = 0; i < size; ++i) {
			const Gaff::JSON& module = module_load_order[i];
			GAFF_ASSERT(module.isString());

			const U8String path = U8String(module.getString()) + BIT_EXTENSION DYNAMIC_EXTENSION;

			// Skip modules that begin with Editor if we're not the editor.
			if (!_editor && path.find("Editor") == 0) {
				continue;
			}

			if (!loadModule(path.data())) {
				LogErrorDefault("Failed to load module '%s'!", path.data());
				return false;
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

				// Skip modules that begin with Editor if we're not the editor.
				if (!_editor && Gaff::FindFirstOf(temp, "Editor") == 0) {
					continue;
				}

				if (!(_dynamic_loader.getModule(temp) || loadModule(temp))) {
					return false;
				}
			}
		}
	}

	// Create manager instances.
	const Vector<const Gaff::IReflectionDefinition*>* manager_bucket = _reflection_mgr.getTypeBucket(Gaff::FNV1aHash64Const("IManager"));

	for (const Gaff::IReflectionDefinition* ref_def : *manager_bucket) {
		ProxyAllocator allocator;
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

	// Notify all modules that every module has been loaded.
	_dynamic_loader.forEachModule([](const HashString32&, DynamicLoader::ModulePtr& module) -> bool
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
			memcpy(temp + wd.size(), TEXT("bin"), sizeof(wchar_t));
		} else {
			memcpy(temp + wd.size(), TEXT("/bin"), sizeof(wchar_t));
		}

		if (!SetDllDirectory(temp)) {
			LogErrorDefault("Failed to set working directory to '%s'.", wd.getString());
			return false;
		}

	} else {
		if (!SetDllDirectory(TEXT("bin"))) {
			LogErrorDefault("Failed to set working directory to '%s'.", wd.getString());
			return false;
		}
#endif
	}

	return true;
}

bool App::loadModule(const char* module_path)
{
	// If it's not a dynamic module,
	// or if it is not compiled for our architecture and build mode,
	// then just skip over it.
	if (!Gaff::File::CheckExtension(module_path, BIT_EXTENSION DYNAMIC_EXTENSION)) {
		return true;
	}

#ifdef PLATFORM_WINDOWS
	DynamicLoader::ModulePtr module = _dynamic_loader.loadModule((U8String("..\\") + module_path).data(), module_path);
#else
	DynamicLoader::ModulePtr module = _dynamic_loader.loadModule(module_path, module_path);
#endif

	if (!module) {
		LogWarningDefault("Failed to find or load dynamic module '%s'.", module_path);
		return false;
	}

#ifdef INIT_STACKTRACE_SYSTEM
	Gaff::StackTrace::RefreshModuleList(); // Will fix symbols from DLLs not resolving.
#endif

	InitModuleFunc init_func = module->getFunc<InitModuleFunc>("InitModule");

	if (!init_func) {
		LogErrorDefault("Failed to find function 'InitModule' in dynamic module '%s'.", module_path);
		return false;
	}
	else if (!init_func(*this)) {
		LogErrorDefault("Failed to initialize dynamic module '%s'.", module_path);
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

void App::run(void)
{
	while (_running) {
		//uv_run(uv_default_loop(), UV_RUN_NOWAIT);
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
		SHIB_FREET(_fs.file_system, GetAllocator());
	}

#ifdef INIT_STACKTRACE_SYSTEM
	Gaff::StackTrace::Destroy();
#endif
}

const IEditor* App::getEditor(void) const
{
	return _editor;
}

IEditor* App::getEditor(void)
{
	return _editor;
}

void App::setEditor(IEditor* editor)
{
	_editor = editor;
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

const U8String& App::getProjectDirectory(void) const
{
	return _project_dir;
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

NS_END
