/************************************************************************************
Copyright (C) 2022 by Nicholas LaCroix

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

#pragma once

#include "Shibboleth_ReflectionManager.h"
#include "Shibboleth_RuntimeVarManager.h"
#include "Shibboleth_ThreadAllocator.h"
#include "Shibboleth_DynamicLoader.h"
//#include "Shibboleth_FileWatcher.h"
#include "Shibboleth_LogManager.h"
#include "Shibboleth_SmartPtrs.h"
#include "Shibboleth_IManager.h"
#include "Shibboleth_IModule.h"
#include "Shibboleth_JobPool.h"
#include "Shibboleth_IApp.h"
#include <Gaff_JSON.h>

#ifdef SHIB_STATIC
namespace Gen::Engine
{
	static bool LoadModulesStatic(Shibboleth::App& app);
}
#endif

NS_SHIBBOLETH

class IMainLoop;

class App final : public IApp
{
public:
	App(void);
	~App(void);

	bool init(int argc, const char** argv);

	void run(void);
	void destroy(void);

	const IManager* getManager(Gaff::Hash64 name) const override;
	IManager* getManager(Gaff::Hash64 name) override;

	IFileSystem& getFileSystem(void) override;
	const Gaff::JSON& getConfigs(void) const override;
	Gaff::JSON& getConfigs(void) override;
	U8String getProjectDirectory(void) const override;

	const ReflectionManager& getReflectionManager(void) const override;
	ReflectionManager& getReflectionManager(void) override;
	LogManager& getLogManager(void) override;
	JobPool& getJobPool(void) override;

	//FileWatcherManager& getFileWatcherManager(void) override;
	DynamicLoader& getDynamicLoader(void) override;

	bool isQuitting(void) const override;
	void quit(void) override;


#ifdef SHIB_RUNTIME_VAR_ENABLED
	RuntimeVarManager& getRuntimeVarManager(void) override;
#endif

private:
	using InitFileSystemModuleFunc = bool (*)(IApp&);
	using CreateModuleFunc = IModule* (*)(void);

	struct FileSystemData
	{
		FileSystemData(void): file_system(nullptr), destroy_func(nullptr), create_func(nullptr) {}

		using InitFileSystemModuleFunc = bool (*)(IApp&);
		using CreateFileSystemFunc = IFileSystem* (*)(void);
		using DestroyFileSystemFunc  = void (*)(IFileSystem*);

		DynamicLoader::ModulePtr file_system_module;
		IFileSystem* file_system;

		DestroyFileSystemFunc destroy_func;
		CreateFileSystemFunc create_func;
	};

	using MainLoopFunc = void (*)(void);

	bool _running = true;
	IMainLoop* _main_loop = nullptr;

	ReflectionManager _reflection_mgr;

	//FileWatcherManager _file_watcher_mgr;
	DynamicLoader _dynamic_loader;
	LogManager _log_mgr;
	JobPool _job_pool{ ProxyAllocator("Job Pool") };

#ifdef SHIB_RUNTIME_VAR_ENABLED
	RuntimeVarManager _runtime_var_mgr;
#endif

	FileSystemData _fs;

	VectorMap< Gaff::Hash64, UniquePtr<IManager> > _manager_map{ ProxyAllocator("Reflection") };
	VectorMap< Gaff::Hash64, UniquePtr<IModule> > _module_map{ ProxyAllocator("Reflection") };
	Gaff::JSON _configs;

	ThreadAllocator _thread_allocator;

	bool initInternal(void);
	bool loadFileSystem(void);
	bool loadMainLoop(void);
	bool loadModules(void);
	bool initApp(void);

	void removeExtraLogs(void);

	bool createManagersInternal(const Vector<const Refl::IReflectionDefinition*>& managers);
	bool hasManager(Gaff::Hash64 name) const;

	bool createModule(CreateModuleFunc create_func, const char8_t* module_name);

	static void ModuleChanged(const char8_t* path);
	static void ThreadInit(uintptr_t thread_id);
	static void ThreadShutdown(uintptr_t thread_id);

	GAFF_NO_COPY(App);
	GAFF_NO_MOVE(App);

#ifdef SHIB_STATIC
	friend bool Gen::Engine::LoadModulesStatic(Shibboleth::App& app);
#endif
};

NS_END
