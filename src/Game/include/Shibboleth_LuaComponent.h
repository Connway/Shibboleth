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

#include <Shibboleth_ReflectionDefinitions.h>
#include <Shibboleth_ResourceDefines.h>
#include <Shibboleth_ResourceWrapper.h>
#include <Shibboleth_IComponent.h>

namespace lua
{
	class State;
}

NS_SHIBBOLETH

class ResourceManager;
class App;

class LuaComponent : public IComponent
{
public:
	LuaComponent(App& app);
	~LuaComponent(void);

	bool load(const Gaff::JSON& json);
	bool save(Gaff::JSON& json);

	void allComponentsLoaded(void);

	static const char* getComponentName(void)
	{
		return "Lua Component";
	}

	static void InitReflectionDefinition(void);

private:
	static ReflectionDefinition<LuaComponent> _ref_def;

	ResourceWrapper< SingleDataWrapper<lua::State*> > _script_res;

	ResourceManager& _res_mgr;
	AString _lua_file;
};

NS_END