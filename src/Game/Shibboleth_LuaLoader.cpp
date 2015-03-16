/************************************************************************************
Copyright (C) 2015 by Nicholas LaCroix

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

#include "Shibboleth_LuaLoader.h"
#include "Shibboleth_ResourceDefines.h"
#include "Shibboleth_LuaManager.h"
#include <Shibboleth_IFileSystem.h>
#include <Shibboleth_Utilities.h>
#include <Shibboleth_IApp.h>
#include <LuaState.h>

NS_SHIBBOLETH

LuaLoader::LuaLoader(LuaManager& lua_manager):
	_lua_manager(lua_manager)
{
}

LuaLoader::~LuaLoader(void)
{
}

Gaff::IVirtualDestructor* LuaLoader::load(const char* file_name, unsigned long long, HashMap<AString, IFile*>& file_map)
{
	assert(file_map.hasElementWithKey(AString(file_name)));
	IFile*& file = file_map[AString(file_name)];

	SingleDataWrapper<lua::State*>* lua_data = GetAllocator()->template allocT< SingleDataWrapper<lua::State*> >();

	if (lua_data) {
		lua_data->data = _lua_manager.createNewState();

		if (lua_data->data) {
			lua_data->data->doString(file->getBuffer());
		}
	}

	GetApp().getFileSystem()->closeFile(file);
	file = nullptr;

	return lua_data;
}

NS_END
