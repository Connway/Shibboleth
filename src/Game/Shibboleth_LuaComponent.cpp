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

#include "Shibboleth_LuaComponent.h"
#include <Shibboleth_ResourceManager.h>
#include <LuaState.h>

NS_SHIBBOLETH

ReflectionDefinition<LuaComponent> LuaComponent::_ref_def;

COMP_REF_DEF_SAVE(LuaComponent, _ref_def);

LuaComponent::LuaComponent(App& app):
	_res_mgr(app.getManager<ResourceManager>("Resource Manager"))
{
}

LuaComponent::~LuaComponent(void)
{
}

bool LuaComponent::load(const Gaff::JSON& json)
{
	_ref_def.read(json, this);
	assert(_lua_file.size());
	_script_res = _res_mgr.requestResource(_lua_file.getBuffer());
	return true;
}

void LuaComponent::allComponentsLoaded(void)
{
}

void LuaComponent::InitReflectionDefinition(void)
{
	if (!_ref_def.isDefined()) {
		_ref_def.setAllocator(ProxyAllocator());

		_ref_def.addANSIString("Lua Filename", &LuaComponent::_lua_file);

		_ref_def.markDefined();
	}
}

NS_END