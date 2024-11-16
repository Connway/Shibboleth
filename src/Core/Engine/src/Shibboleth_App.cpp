/************************************************************************************
Copyright (C) 2024 by Nicholas LaCroix

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
#include "Attributes/Shibboleth_EngineAttributesCommon.h"
#include "Config/Shibboleth_EngineConfig.h"
#include "FileSystem/Shibboleth_LooseFileSystem.h"
#include "Shibboleth_SerializeReader.h"
#include "Shibboleth_AppConfigs.h"
#include "Shibboleth_IMainLoop.h"
#include "Gen_StaticReflectionInit.h"
#include <Gaff_CrashHandler.h>
#include <Gaff_Utils.h>
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

	return initApp();
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

	const EngineConfig& engine_config = GetConfigRef<EngineConfig>();

	// Free managers and shutdown modules in requested to close these first.
	for (const HashString64<>& module_name : engine_config.module_unload_order) {
		// Find all the managers for this module and free them.
		const auto* const manager_refls = _reflection_mgr.getTypeBucket<IManager>(module_name.getHash());

		if (manager_refls) {
			for (const Refl::IReflectionDefinition* ref_def : *manager_refls) {
				const auto it = _manager_map.find(ref_def->getReflectionInstance().getNameHash());

				if (it != _manager_map.end()) {
					_manager_map.erase(it);
				}
			}
		}

		const auto module_ptr_it = _module_map.find(module_name.getHash());

		if (module_ptr_it != _module_map.end()) {
			module_ptr_it->second->shutdown();
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

// Still single-threaded at this point, so ok that we're not using the spinlock
bool App::loadFileSystem(void)
{
	const EngineConfig& engine_config = GetConfigRef<EngineConfig>();

	if (engine_config.file_system.empty()) {
		LogInfoDefault("Defaulting to loose file system.");

		_fs.file_system = SHIB_ALLOCT(LooseFileSystem, GetAllocator());

		if (!_fs.file_system) {
			LogErrorDefault("Failed to create loose file system.");
			return false;
		}

	} else {
		const U8String fs = engine_config.file_system + /*TARGET_SUFFIX_U8*/ DYNAMIC_MODULE_EXTENSION_U8;

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
	}

	return true;
}

bool App::loadMainLoop(void)
{
	const EngineConfig& engine_config = GetConfigRef<EngineConfig>();

	if (engine_config.flags.testAll(EngineConfig::Flag::NoMainLoop)) {
		return true;
	}

	const Vector<const Refl::IReflectionDefinition*>* const bucket = _reflection_mgr.getTypeBucket(CLASS_HASH(Shibboleth::IMainLoop));

	if (!bucket || bucket->empty()) {
		LogErrorDefault("App::loadMainLoop: Failed to find a class that implements the 'IMainLoop' interface.");
		return false;
	}

	const Refl::IReflectionDefinition* const refl = engine_config.main_loop ? engine_config.main_loop : bucket->front();

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
	const EngineConfig& engine_config = GetConfigRef<EngineConfig>();

	if (engine_config.flags.testAll(EngineConfig::Flag::NoLoadModules)) {
		_job_pool.run();
		return true;
	}

#ifdef SHIB_STATIC
	if (!Gen::Engine::LoadModulesStatic()) {
		return false;
	}

#else
	if (engine_config.module_directories.empty()) {
		LogErrorDefault("App::loadModules: EngineConfig::module_directories is empty.");
		return false;

	} else {
		const char8_t separator = Gaff::ConvertChar<char8_t>(std::filesystem::path::preferred_separator);

		for (const U8String& dir : engine_config.module_directories) {
			for (const auto& dir_entry : std::filesystem::directory_iterator(dir.data())) {
				if (!dir_entry.is_regular_file()) {
					continue;
				}

				const auto abs_path = std::filesystem::absolute(dir_entry.path());
				const auto* name = abs_path.c_str();
				CONVERT_STRING(char8_t, temp, name);

				U8String module_name(temp);
				const size_t pos = module_name.rfind(separator);

				GAFF_ASSERT(pos != U8String::npos);

				module_name = module_name.substr(pos + 1, module_name.size() - eastl::CharStrlen(u8"Module" /*TARGET_SUFFIX_U8*/ DYNAMIC_MODULE_EXTENSION_U8) - (pos + 1));

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
	}
#endif

	_module_map.shrink_to_fit();

	// Run module pre-init.
	for (auto& entry : _module_map) {
		if (!entry.second->preInit(*this)) {
			//LogErrorDefault("Module::preInit() failed for module '%s'.", entry.first.getBuffer());
			return false;
		}
	}

	// Init owned enums and attributes reflection.
	for (auto& entry : _module_map) {
		entry.second->initReflectionEnums();
	}

	for (auto& entry : _module_map) {
		entry.second->initReflectionAttributes();
	}

	// Init non-owned enums and attributes reflection.
	for (auto& entry : _module_map) {
		entry.second->initNonOwnedEnums();
	}

	for (auto& entry : _module_map) {
		entry.second->initNonOwnedAttributes();
	}

	// Init owned classes reflection.
	for (auto& entry : _module_map) {
		entry.second->initReflectionClasses();
	}

	// Init non-owned classes reflection.
	for (auto& entry : _module_map) {
		entry.second->initNonOwnedClasses();
	}

	// Initialize any newly loaded configs.
	if (!createConfigs()) {
		return false;
	}

	// Create manager instances.
	if (!engine_config.flags.testAll(EngineConfig::Flag::NoManagers)) {
		// Create managers from module load order first.
		for (const HashString64<>& module_name : engine_config.module_load_order) {
			const Vector<const Refl::IReflectionDefinition*>* const manager_bucket = _reflection_mgr.getTypeBucket<IManager>(module_name);

			if (manager_bucket) {
				if (!createManagersInternal(*manager_bucket)) {
					return false;
				}
			}
		}

		// Create the rest of the managers.
		const Vector<const Refl::IReflectionDefinition*>* const manager_bucket = _reflection_mgr.getTypeBucket<IManager>();

		if (manager_bucket) {
			if (!createManagersInternal(*manager_bucket)) {
				return false;
			}
		}
	}

	_manager_map.shrink_to_fit();

	// Run module post-init.
	for (auto& entry : _module_map) {
		if (!entry.second->postInit()) {
			//LogErrorDefault("Module::postInit() failed for module '%s'.", entry.first.getBuffer());
			return false;
		}
	}

	_job_pool.run();

	// Call initAllModulesLoaded() on all managers.
	Vector<const Refl::IReflectionDefinition*> already_initialized_managers;

	// Call on module load order first.
	for (const HashString64<>& module_name : engine_config.module_load_order) {
		const Vector<const Refl::IReflectionDefinition*>* const manager_bucket = _reflection_mgr.getTypeBucket<IManager>(module_name.getHash());

		if (manager_bucket) {
			for (const Refl::IReflectionDefinition* ref_def : *manager_bucket) {
				if (!_manager_map[ref_def->getReflectionInstance().getNameHash()]->initAllModulesLoaded()) {
					LogErrorDefault("Failed to initialize manager after all modules loaded '%s'!", ref_def->getReflectionInstance().getName());
					return false;
				}
			}

			already_initialized_managers.insert(already_initialized_managers.end(), manager_bucket->begin(), manager_bucket->end());
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
	EA::Thread::SetAllocator(&_thread_allocator);

	_reflection_mgr.registerTypeBucket(CLASS_HASH(Shibboleth::IMainLoop));
	_reflection_mgr.registerTypeBucket<Refl::IAttribute>();

	// Init engine reflection.
	for (int32_t mode_count = 0; mode_count < static_cast<int32_t>(Gen::InitMode::Count); ++mode_count) {
		Gen::Engine::InitReflection(static_cast<Gen::InitMode>(mode_count));
	}

	LogInfoDefault("Initializing...");

	// Initialize engine configs.
	if (!createConfigs()) {
		_log_mgr.init(nullptr); // Attempt to intitialize the logger. So that we write SOMETHING.
		return false;
	}

	const EngineConfig& engine_config = GetConfigRef<EngineConfig>();

	if (!_log_mgr.init(engine_config.log_dir.empty() ? nullptr : engine_config.log_dir.data())) {
		return false;
	}

	if (!engine_config.working_dir.empty()) {
		if (!Gaff::SetWorkingDir(engine_config.working_dir.data())) {
			LogErrorDefault("Failed to set working directory to '%s'.", engine_config.working_dir.data());
			return false;
		}

		// Set DLL auto-load directory.
#ifdef PLATFORM_WINDOWS
		U8String bin_dir{ ENGINE_ALLOCATOR };
		bin_dir = engine_config.working_dir;

		if (bin_dir.back() != u8'/' && bin_dir.back() != u8'\\') {
			bin_dir.push_back(u8'/');
		}

		bin_dir += u8"bin";

		CONVERT_STRING(wchar_t, temp, bin_dir.data());

		if (!SetDllDirectory(temp)) {
			LogErrorDefault("Failed to set DLL directory to '%ls'.", temp);
			return false;
		}
#endif

#ifdef PLATFORM_WINDOWS
	} else {
		if (!SetDllDirectory(L"bin")) {
			LogErrorDefault("Failed to set DLL directory to 'bin'.");
			return false;
		}
#endif
	}

	if (!_job_pool.init(static_cast<int32_t>(Gaff::GetNumberOfCores()), App::ThreadInit, App::ThreadShutdown)) {
		LogErrorDefault("Failed to initialize thread pool.");
		return false;
	}

	_job_pool.addPool(HashStringView32<>(EngineConfig::k_read_file_pool_name), engine_config.read_file_threads);

	if (!loadFileSystem()) {
		return false;
	}

	if (!loadModules()) {
		return false;
	}

	// Reload engine config now that all modules are loaded.
	if (InitConfig<EngineConfig>().isFatal()) {
		return false;
	}

	if (!loadMainLoop()) {
		return false;
	}

	LogInfoDefault("Game Successfully Initialized.");
	return true;
}

bool App::createManagersInternal(const Vector<const Refl::IReflectionDefinition*>& managers)
{
	ProxyAllocator allocator;

	for (const Refl::IReflectionDefinition* ref_def : managers) {
		if (hasManager(ref_def->getReflectionInstance().getNameHash())) {
			continue;
		}

		IManager* const manager = ref_def->template createT<IManager>(allocator);

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

bool App::createConfigs(void)
{
	const auto* const configs = _reflection_mgr.getAttributeBucket<GlobalConfigAttribute>();

	if (!configs) {
		return true;
	}

	for (const Refl::IReflectionDefinition* ref_def : *configs) {
		GlobalConfigAttribute* const attr = const_cast<GlobalConfigAttribute*>(ref_def->getClassAttr<GlobalConfigAttribute>());

		if (attr->getConfig()) {
			continue;
		}

		if (const auto error = attr->createAndLoadConfig(*ref_def); error) {
			if (error.isFatal()) {
				return false;
			}

			continue;
		}
	}

	return true;
}

void App::ModuleChanged(const char8_t* path)
{
	if (!Gaff::EndsWith(path, /*TARGET_SUFFIX_U8*/ DYNAMIC_MODULE_EXTENSION_U8)) {
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
			LogErrorDefault("Failed to initialize thread for '%s'.", entry.second->getReflectionDefinition().getReflectionInstance().getName());

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
