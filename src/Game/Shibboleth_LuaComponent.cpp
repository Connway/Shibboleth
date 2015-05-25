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

#include "Shibboleth_LuaComponent.h"
#include <Shibboleth_ResourceManager.h>
#include <Shibboleth_Utilities.h>
#include <Shibboleth_IApp.h>
#include <LuaState.h>

NS_SHIBBOLETH

COMP_REF_DEF_SAVE(LuaComponent, gRefDef);
REF_IMPL_REQ(LuaComponent);

SHIB_REF_IMPL(LuaComponent)
.addBaseClassInterfaceOnly<LuaComponent>()
.addString("Lua Filename", &LuaComponent::_lua_file)
;

LuaComponent::LuaComponent(void)
{
}

LuaComponent::~LuaComponent(void)
{
}

bool LuaComponent::load(const Gaff::JSON& json)
{
	gRefDef.read(json, this);
	assert(_lua_file.size());

	ResourceManager& res_mgr = Shibboleth::GetApp().getManagerT<ResourceManager>("Resource Manager");

	_script_res = res_mgr.requestResource(_lua_file.getBuffer());
	return true;
}

void LuaComponent::allComponentsLoaded(void)
{
}

NS_END
