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

#include "Shibboleth_Game.h"
#include "Shibboleth_String.h"
#include <Gaff_Utils.h>
#include <Gaff_File.h>
#include <Gaff_JSON.h>
#include <iostream>

NS_SHIBBOLETH

Gaff::DefaultAllocator Game::_allocator;

Game::Game(void):
	_manager_map(_allocator)
{
}

Game::~Game(void)
{
	for (ManagerMap::Iterator it = _manager_map.begin(); it != _manager_map.end(); ++it) {
		it->destroy_func(&_allocator, it->manager);
	}
}

bool Game::init(void)
{
	Gaff::JSON::SetMemoryFunctions(&Game::ShibbolethAllocate, &Game::ShibbolethFree);

	loadManagers();

	if (!loadStates()) {
		return false;
	}

	return true;
}

void Game::loadManagers(void)
{
	// Load managers from DLLs
	Gaff::ForEachTypeInDirectory<Gaff::DT_RegularFile>("./Managers", [&](const char* name, size_t name_len) -> bool
	{
		if (!Gaff::File::checkExtension(name, DYNAMIC_EXTENSION)) {
			return false;
		}

		AString rel_path = AString("./Managers/", _allocator) + name;

		DynamicLoader::ModulePtr module = _dynamic_loader.loadModule(rel_path.getBuffer(), name);

		if (module) {
			ManagerEntry entry = { nullptr, nullptr, nullptr };
			entry.create_func = module->GetFunc<ManagerEntry::CreateManagerFunc>("CreateManager");
			entry.destroy_func = module->GetFunc<ManagerEntry::DestroyManagerFunc>("DestroyManager");

			if (entry.create_func && entry.destroy_func) {
				entry.manager = entry.create_func(&_allocator);

				if (entry.manager) {
					Gaff::AHashString<Gaff::DefaultAllocator> hstr(entry.manager->GetName(), Gaff::FNVHash, _allocator);
					_manager_map[hstr] = entry;
				}
			} else {
				_dynamic_loader.removeModule(name);
			}
		}

		return false;
	});
}

bool Game::loadStates(void)
{
	Gaff::JSON state_data;
	
	if (!state_data.parseFile("./States/states.json")) {
		return false;
	}

	if (!state_data.isArray()) {
		return false;
	}

	for (size_t i = 0; i < state_data.size(); ++i) {
		Gaff::JSON val = state_data[i];
		assert(val.isString());

		AString filename("./States/", _allocator);
		filename += val.getString();
		filename += DYNAMIC_EXTENSION;
		
		DynamicLoader::ModulePtr module = _dynamic_loader.loadModule(filename, val.getString());

		if (module) {
			StateEntry entry = { nullptr, nullptr, nullptr };
			entry.create_func = module->GetFunc<StateEntry::CreateStateFunc>("CreateState");
			entry.destroy_func = module->GetFunc<StateEntry::DestroyStateFunc>("DestroyState");

			if (entry.create_func && entry.destroy_func) {
				entry.state = entry.create_func(&_allocator);

				if (entry.state) {
					_states.push(entry);
				}
			} else {
				_dynamic_loader.removeModule(val.getString());
			}
		}
	}

	return true;
}

void* Game::ShibbolethAllocate(size_t size)
{
	return _allocator.alloc((unsigned int)size);
}

void Game::ShibbolethFree(void* data)
{
	_allocator.free(data);
}

NS_END
