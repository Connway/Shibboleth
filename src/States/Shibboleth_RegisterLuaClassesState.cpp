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

#include "Shibboleth_RegisterLuaClassesState.h"
#include <Shibboleth_ILuaManager.h>
#include <Shibboleth_IApp.h>

#ifdef PLATFORM_WINDOWS
	#pragma warning(push)
	#pragma warning(disable: 4100 4244 4267 4800)
#endif

#include <LuaState.h>
#include <LuaBridge.h>

#ifdef PLATFORM_WINDOWS
	#pragma warning(pop)
#endif

NS_SHIBBOLETH

static void RegisterBaseClassesWithLua(lua::State& /*state*/)
{

}

RegisterLuaClassesState::RegisterLuaClassesState(IApp& app):
	_app(app)
{
}

RegisterLuaClassesState::~RegisterLuaClassesState(void)
{
}

bool RegisterLuaClassesState::init(unsigned int)
{
	return true;
}

void RegisterLuaClassesState::enter(void)
{
}

void RegisterLuaClassesState::update(void)
{
	_app.getManagerT<ILuaManager>().addRegistrant(Gaff::Bind(RegisterBaseClassesWithLua));
}

void RegisterLuaClassesState::exit(void)
{
}

NS_END
