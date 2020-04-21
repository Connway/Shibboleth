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

#include "Shibboleth_LuaResource.h"
#include <Shibboleth_LoadFileCallbackAttribute.h>
#include <Shibboleth_ResourceAttributesCommon.h>
#include <Shibboleth_IFileSystem.h>
#include <Shibboleth_LuaManager.h>
#include <Shibboleth_LogManager.h>
#include <Shibboleth_Utilities.h>
#include <lua.hpp>

SHIB_REFLECTION_DEFINE_BEGIN(LuaResource)
	.classAttrs(
		//ResExtAttribute(".lua.bin"),
		ResExtAttribute(".lua"),
		MakeLoadFileCallbackAttribute(&LuaResource::loadScript)
	)

	.base<IResource>()
	.ctor<>()
SHIB_REFLECTION_DEFINE_END(LuaResource)

NS_SHIBBOLETH

SHIB_REFLECTION_CLASS_DEFINE(LuaResource)

LuaResource::~LuaResource(void)
{
	LuaManager& lua_mgr = GetApp().getManagerTFast<LuaManager>();
	lua_mgr.unloadBuffer(getFilePath().getBuffer());
}

void LuaResource::loadScript(IFile* file)
{
	LuaManager& lua_mgr = GetApp().getManagerTFast<LuaManager>();

	if (lua_mgr.loadBuffer(reinterpret_cast<const char*>(file->getBuffer()), file->size(), getFilePath().getBuffer())) {
		succeeded();

	} else {
		// $TODO: Log error.
		failed();
	}
}

NS_END
