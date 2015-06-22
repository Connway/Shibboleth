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

#pragma once

#include <Shibboleth_ReflectionDefinitions.h>
#include <Shibboleth_IManager.h>
#include <Shibboleth_Array.h>
#include <Gaff_Function.h>

namespace lua
{
	class State;
}

NS_SHIBBOLETH

class LuaManager : public IManager
{
public:
	LuaManager(void);
	~LuaManager(void);

	const char* getName(void) const;

	void addRegistrant(const Gaff::FunctionBinder<void, lua::State&>& registrant);

	lua::State* createNewState(void);

	void* rawRequestInterface(unsigned int class_id) const;

private:
	Array< Gaff::FunctionBinder<void, lua::State&> > _registrants;

	GAFF_NO_COPY(LuaManager);
	GAFF_NO_MOVE(LuaManager);

	SHIB_REF_DEF(LuaManager);
};

NS_END
