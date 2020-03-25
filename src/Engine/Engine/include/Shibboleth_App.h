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

#pragma once

#include "Shibboleth_ReflectionManager.h"
#include "Shibboleth_ThreadAllocator.h"
#include "Shibboleth_DynamicLoader.h"
#include "Shibboleth_Broadcaster.h"
#include "Shibboleth_LogManager.h"
#include "Shibboleth_SmartPtrs.h"
#include "Shibboleth_JobPool.h"
#include "Shibboleth_IApp.h"
#include <Gaff_JSON.h>

NS_SHIBBOLETH

class IMainLoop;

class App final : public IApp
{
public:
	App(void);
	~App(void);

	bool init(int argc, const char** argv);
#ifdef PLATFORM_WINDOWS
	bool init(void);
#endif

	void run(void);
	void destroy(void);

	const IManager* getManager(Gaff::Hash64 name) const override;
	IManager* getManager(Gaff::Hash64 name) override;

	IFileSystem& getFileSystem(void) override;
	const Gaff::JSON& getConfigs(void) const override;
	Gaff::JSON& getConfigs(void) override;
	U8String getProjectDirectory(void) const override;

	const IReflectionManager& getReflectionManager(void) const override;
	IReflectionManager& getReflectionManager(void) override;
	Broadcaster& getBroadcaster(void) override;
	LogManager& getLogManager(void) override;
	JobPool& getJobPool(void) override;

	DynamicLoader& getDynamicLoader(void) override;

	bool isQuitting(void) const override;
	void quit(void) override;

private:
	using InitFileSystemModuleFunc = bool (*)(IApp&);
	using InitModuleFunc = bool (*)(IApp&, InitMode);

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

	DynamicLoader _dynamic_loader;
	Broadcaster _broadcaster;
	LogManager _log_mgr;
	JobPool _job_pool{ ProxyAllocator("Job Pool") };

	FileSystemData _fs;

	VectorMap< Gaff::Hash64, UniquePtr<IManager> > _manager_map{ ProxyAllocator("Reflection") };
	Gaff::JSON _configs;

	ThreadAllocator _thread_allocator;

	bool initInternal(void);
	bool loadFileSystem(void);
	bool loadMainLoop(void);
	bool loadModules(void);
	bool initApp(void);

	bool loadModule(const char* module_name, InitMode mode);
	bool createManagers(void);

	void removeExtraLogs(void);

	bool createManagersInternal(const Vector<const Gaff::IReflectionDefinition*>& managers);
	bool hasManager(Gaff::Hash64 name) const;

	GAFF_NO_COPY(App);
	GAFF_NO_MOVE(App);
};

NS_END
