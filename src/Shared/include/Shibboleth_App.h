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

#pragma once

#include "Shibboleth_MessageBroadcaster.h"
#include "Shibboleth_LogManager.h"
#include "Shibboleth_IApp.h"

NS_SHIBBOLETH

class App : public IApp
{
public:
	template <class Callback>
	void forEachManager(Callback&& callback)
	{
		for (auto it = _manager_map.begin(); it != _manager_map.end(); ++it) {
			if (callback(*it->manager)) {
				break;
			}
		}
	}

	App(void);
	~App(void);

	bool init(int argc, char** argv);
	void run(void);
	void destroy(void);

	const IManager* getManager(Gaff::Hash32 name) const override;
	IManager* getManager(Gaff::Hash32 name) override;

	MessageBroadcaster& getBroadcaster(void) override;

	IFileSystem* getFileSystem(void) override;
	const VectorMap<HashString32, U8String>& getCmdLine(void) const override;
	VectorMap<HashString32, U8String>& getCmdLine(void) override;

	LogManager& getLogManager(void) override;
	JobPool& getJobPool(void) override;

	DynamicLoader::ModulePtr loadModule(const char* filename, const char* name) override;

	const Gaff::IReflectionDefinition* getReflection(Gaff::Hash64 name) const override;
	void registerReflection(Gaff::Hash64 name, Gaff::IReflectionDefinition* ref_def) override;

	bool isQuitting(void) const override;
	void quit(void) override;

private:
	struct ManagerEntry
	{
		typedef IManager* (*CreateManagerFunc)(unsigned int);
		typedef void (*DestroyManagerFunc)(IManager*, unsigned int);
		typedef unsigned int (*GetNumManagersFunc)(void);
		typedef bool (*InitManagerModuleFunc)(IApp&);

		DynamicLoader::ModulePtr module;
		CreateManagerFunc create_func;
		DestroyManagerFunc destroy_func;
		IManager* manager;
		unsigned int manager_id;
	};

	struct FileSystemData
	{
		FileSystemData(void): file_system(nullptr), destroy_func(nullptr), create_func(nullptr) {}

		typedef bool (*InitFileSystemModuleFunc)(IApp&);
		typedef IFileSystem* (*CreateFileSystemFunc)(void);
		typedef void (*DestroyFileSystemFunc)(IFileSystem*);

		DynamicLoader::ModulePtr file_system_module;
		IFileSystem* file_system;

		DestroyFileSystemFunc destroy_func;
		CreateFileSystemFunc create_func;
	};

	using ManagerMap = VectorMap<Gaff::Hash32, ManagerEntry>;
	using MainLoopFunc = void (*)(void);

	bool _running;
	MainLoopFunc _main_loop;
	VectorMap< Gaff::Hash64, UniquePtr<Gaff::IReflectionDefinition> > _reflection_map;

	MessageBroadcaster _broadcaster;
	DynamicLoader _dynamic_loader;
	ManagerMap _manager_map;
	JobPool _job_pool;
	LogManager _logger;

	FileSystemData _fs;

	VectorMap<HashString32, U8String> _cmd_line_args;

	bool loadFileSystem(void);
	bool loadManagers(void);
	bool loadMainLoop(void);
	bool initApp(void);

	void removeExtraLogs(void);

	GAFF_NO_COPY(App);
	GAFF_NO_MOVE(App);
};

NS_END
