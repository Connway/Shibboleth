/************************************************************************************
Copyright (C) 2014 by Nicholas LaCroix

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
#include "Shibboleth_StateMachine.h"
#include "Shibboleth_ThreadPool.h"
#include "Shibboleth_HashString.h"
#include "Shibboleth_HashMap.h"
#include "Shibboleth_IState.h"
#include "Shibboleth_Logger.h"
#include "Shibboleth_Array.h"
#include <Gaff_INamedObject.h>

NS_SHIBBOLETH

class App
{
public:
	template <class T>
	const T& getManager(const AHashString& name) const
	{
		assert(name.size() && _manager_map.indexOf(name) != -1);
		return *(T*)_manager_map[name].manager;
	}

	template <class T>
	T& getManager(const AHashString& name)
	{
		assert(name.size() && _manager_map.indexOf(name) != -1);
		return *(T*)_manager_map[name].manager;
	}

	template <class T>
	const T& getManager(const AString& name) const
	{
		assert(name.size() && _manager_map.indexOf(name) != -1);
		return *(T*)_manager_map[name].manager;
	}

	template <class T>
	T& getManager(const AString& name)
	{
		assert(name.size() && _manager_map.indexOf(name) != -1);
		return *(T*)_manager_map[name].manager;
	}

	template <class T>
	const T& getManager(const char* name) const
	{
		assert(name && _manager_map.indexOf(name) != -1);
		return *(T*)_manager_map[name].manager;
	}

	template <class T>
	T& getManager(const char* name)
	{
		assert(name && _manager_map.indexOf(name) != -1);
		return *(T*)_manager_map[name].manager;
	}

	App(void);
	~App(void);

	bool init(void);
	void run(void);

	INLINE Allocator& getAllocator(void);
	INLINE LogManager& getLogManager(void);

	INLINE void addTask(Gaff::ITask<ProxyAllocator>* task);
	INLINE MessageBroadcaster& getBroadcaster(void);
	INLINE StateMachine& getStateMachine(void);

	INLINE DynamicLoader& getDynamicLoader(void);
	INLINE LogManager::FileLockPair& getGameLogFile(void);

	INLINE size_t getSeed(void) const;

	INLINE void quit(void);

private:
	struct ManagerEntry
	{
		typedef Gaff::INamedObject* (*CreateManagerFunc)(unsigned int);
		typedef void (*DestroyManagerFunc)(Gaff::INamedObject*, unsigned int);
		typedef unsigned int (*GetNumManagersFunc)(void);
		typedef bool (*InitManagerDLLFunc)(App&);

		DynamicLoader::ModulePtr module;
		CreateManagerFunc create_func;
		DestroyManagerFunc destroy_func;
		Gaff::INamedObject* manager;
		unsigned int manager_id;
	};

	typedef HashMap<AHashString, ManagerEntry> ManagerMap;

	Allocator _allocator;

	MessageBroadcaster _broadcaster;
	DynamicLoader _dynamic_loader;
	StateMachine _state_machine;
	ManagerMap _manager_map;
	ThreadPool _thread_pool;

	LogManager _logger;

	LogManager::FileLockPair* _log_file_pair;

	size_t _seed;

	bool _running;

	bool loadManagers(void);
	bool loadStates(void);

	GAFF_NO_COPY(App);
	GAFF_NO_MOVE(App);
};

NS_END
