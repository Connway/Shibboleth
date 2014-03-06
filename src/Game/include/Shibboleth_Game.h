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

#include "Shibboleth_DynamicLoader.h"
#include "Shibboleth_Registry.h"
#include "Shibboleth_IState.h"
#include "Shibboleth_Array.h"
#include <Gaff_INamedObject.h>

NS_SHIBBOLETH

class Game
{
public:
	Game(void);
	~Game(void);

	bool init(void);

private:
	struct ManagerEntry
	{
		typedef Gaff::INamedObject* (*CreateManagerFunc)(Gaff::IAllocator*);
		typedef void (*DestroyManagerFunc)(Gaff::IAllocator*, Gaff::INamedObject*);

		CreateManagerFunc create_func;
		DestroyManagerFunc destroy_func;
		Gaff::INamedObject* manager;
	};

	struct StateEntry
	{
		typedef IState* (*CreateStateFunc)(Gaff::IAllocator*);
		typedef void (*DestroyStateFunc)(Gaff::IAllocator*, IState*);

		CreateStateFunc create_func;
		DestroyStateFunc destroy_func;
		IState* state;
	};

	typedef Gaff::HashMap<Gaff::AHashString<Gaff::DefaultAllocator>, ManagerEntry, Gaff::DefaultAllocator> ManagerMap;

	ManagerMap _manager_map;

	DynamicLoader _dynamic_loader;
	//Registry _manager_registry;
	Array<StateEntry> _states;

	void loadManagers(void);
	bool loadStates(void);

	static Gaff::DefaultAllocator _allocator;
	static void* ShibbolethAllocate(size_t size);
	static void ShibbolethFree(void* data);

	GAFF_NO_COPY(Game);
	GAFF_NO_MOVE(Game);
};

NS_END
