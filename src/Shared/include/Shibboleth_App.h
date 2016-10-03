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
#include "Shibboleth_DynamicLoader.h"
#include "Shibboleth_HashString.h"
#include "Shibboleth_LogManager.h"
#include "Shibboleth_JobPool.h"
#include "Shibboleth_Array.h"
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

	const IManager* getManager(const AHashString& name) const;
	const IManager* getManager(const AString& name) const;
	const IManager* getManager(const char* name) const;
	IManager* getManager(const AHashString& name);
	IManager* getManager(const AString& name);
	IManager* getManager(const char* name);

	MessageBroadcaster& getBroadcaster(void);

	const Array<unsigned int>& getStateTransitions(unsigned int state_id);
	unsigned int getStateID(const char* name);
	void switchState(unsigned int state);

	IFileSystem* getFileSystem(void);
	const HashMap<AHashString, AString>& getCmdLine(void) const;
	HashMap<AHashString, AString>& getCmdLine(void);

	JobPool& getJobPool(void);

	void getWorkerThreadIDs(Array<unsigned int>& out) const;
	void helpUntilNoJobs(void);
	void doAJob(void);

	const char* getLogFileName(void) const;
	LogManager& getLogManager(void);

	DynamicLoader::ModulePtr loadModule(const char* filename, const char* name);

	bool isQuitting(void) const;
	void quit(void);

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

	using ManagerMap = HashMap<AHashString, ManagerEntry>;
	using MainLoopFunc = void (*)(void);

	bool _running;
	MainLoopFunc _main_loop;

	MessageBroadcaster _broadcaster;
	DynamicLoader _dynamic_loader;
	ManagerMap _manager_map;
	JobPool _job_pool;
	LogManager _logger;
	char _log_file_name[64];

	FileSystemData _fs;

	HashMap<AHashString, AString> _cmd_line_args;

	bool loadFileSystem(void);
	bool loadManagers(void);
	bool loadMainLoop(void);
	bool initApp(void);

	void removeExtraLogs(void);

	GAFF_NO_COPY(App);
	GAFF_NO_MOVE(App);
};

NS_END
