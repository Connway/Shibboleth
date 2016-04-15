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

#pragma once

#include <Shibboleth_Memory.h>

#define CREATE_MODULE_TYPE(ReturnType, FuncName) \
	template <class CreateType> \
	ReturnType* FuncName(void) \
	{ \
		return SHIB_ALLOCT(CreateType, *GetAllocator()); \
	}

#define CREATE_MODULE_TYPE_WITH_INIT(ReturnType, FuncName) \
	template <class CreateType> \
	ReturnType* FuncName(void) \
	{ \
		CreateType* type = SHIB_ALLOCT(CreateType, *Shibboleth::GetAllocator()); \
		if (type) { \
			if (!type->init()) { \
				SHIB_FREET(type, *GetAllocator()); \
				type = nullptr; \
			} \
		} \
		return type; \
	}


NS_SHIBBOLETH

class Component;
class IManager;
class IState;

CREATE_MODULE_TYPE_WITH_INIT(IManager, CreateManagerWithInitT);

CREATE_MODULE_TYPE(Component, CreateComponentT);
CREATE_MODULE_TYPE(IManager, CreateManagerT);
CREATE_MODULE_TYPE(IState, CreateStateT);

using ModuleTypeNameFunc = const char* (*)(void);

using CreateComponentFunc = Component* (*)(void);
using CreateManagerFunc = IManager* (*)(void);
using CreateStateFunc = IState* (*)(void);

template <class CreateFunc>
struct ModuleTypeFuncs
{
	CreateFunc create;
	ModuleTypeNameFunc name;
};

using ComponentFuncs = ModuleTypeFuncs<CreateComponentFunc>;
using StateFuncs = ModuleTypeFuncs<CreateStateFunc>;

NS_END
