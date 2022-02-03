/************************************************************************************
Copyright (C) 2022 by Nicholas LaCroix

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

#include <Shibboleth_CommonHelpers.h>
#include <Shibboleth_LuaResource.h>
#include <Esprit_IProcess.h>

NS_SHIBBOLETH

class LuaManager;

class LuaProcess final : public Esprit::IProcess, public Refl::IReflectionObject
{
public:
	bool init(const Esprit::StateMachine& owner) override;
	void update(const Esprit::StateMachine& owner, Esprit::VariableSet::Instance& variables) override;

private:
	TableState _table_state;

	LuaResourcePtr _script;
	LuaManager* _lua_mgr = nullptr;
	bool _log_error = true;

	SHIB_REFLECTION_CLASS_DECLARE(LuaProcess);
};

NS_END

SHIB_REFLECTION_DECLARE(Shibboleth::LuaProcess)
