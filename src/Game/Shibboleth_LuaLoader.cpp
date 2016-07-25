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

#include "Shibboleth_LuaLoader.h"
#include "Shibboleth_ResourceDefines.h"
#include "Shibboleth_ILuaManager.h"
#include <Shibboleth_IFileSystem.h>
#include <Shibboleth_Utilities.h>
#include <Shibboleth_IApp.h>
#include <Gaff_ScopedExit.h>

#ifdef PLATFORM_WINDOWS
	#pragma warning(push)
	#pragma warning(disable: 4100 4244 4267 4800)
#endif

#include <LuaState.h>

#ifdef PLATFORM_WINDOWS
	#pragma warning(pop)
#endif

NS_SHIBBOLETH

LuaLoader::LuaLoader(void)
{
}

LuaLoader::~LuaLoader(void)
{
}

Gaff::IVirtualDestructor* LuaLoader::load(const char* file_name, uint64_t, HashMap<AString, IFile*>& file_map)
{
	GAFF_ASSERT(file_map.hasElementWithKey(AString(file_name)));
	IFile*& file = file_map[AString(file_name)];

	GAFF_SCOPE_EXIT([&]()
	{
		GetApp().getFileSystem()->closeFile(file);
		file = nullptr;
	});

	SingleDataWrapperFree<lua::State>* lua_data = SHIB_ALLOCT(SingleDataWrapperFree<lua::State>, *GetAllocator());

	if (!lua_data) {
		GetApp().getLogManager().logMessage(LogManager::LOG_ERROR, GetApp().getLogFileName(), "ERROR - Failed to allocate memory for Lua data.\n");
		return nullptr;
	}

	lua_data->data = GetApp().getManagerT<ILuaManager>().createNewState();

	if (!lua_data->data) {
		GetApp().getLogManager().logMessage(LogManager::LOG_ERROR, GetApp().getLogFileName(), "ERROR - Failed to create a Lua state for file '%s'.\n", file_name);
		SHIB_FREET(lua_data, *GetAllocator());
		return nullptr;
	}

	// Need to copy to temp to add null-terminator
	AString temp(file->getBuffer(), file->size());
	lua_data->data->doString(temp.getBuffer());

	return lua_data;
}

NS_END
