/************************************************************************************
Copyright (C) 2017 by Nicholas LaCroix

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

#include "Shibboleth_AngelScriptComponentWrapper.h"
#include "Shibboleth_AngelScriptComponent.h"
//#include "Shibboleth_AngelScript.h"
#include <angelscript.h>

NS_SHIBBOLETH

static void* WrapperFactory(void)
{
	IAllocator* const allocator = GetAllocator();
	int32_t pool_index = allocator->getPoolIndex("AngelScript");

	AngelScriptComponentWrapper* const instance = SHIB_ALLOCT_POOL(AngelScriptComponentWrapper, pool_index, *allocator);
	instance->addRef();
	return instance;
}

void AngelScriptComponentWrapper::Register(asIScriptEngine* engine)
{
	engine->RegisterObjectType("ScriptComponent", 0, asOBJ_REF);
	engine->RegisterObjectBehaviour("ScriptComponent", asBEHAVE_FACTORY, "ScriptComponent@ f()", asFUNCTION(WrapperFactory), asCALL_CDECL);
	engine->RegisterObjectBehaviour("ScriptComponent", asBEHAVE_ADDREF, "void f()", asMETHOD(AngelScriptComponentWrapper, addRef), asCALL_THISCALL);
	engine->RegisterObjectBehaviour("ScriptComponent", asBEHAVE_RELEASE, "void f()", asMETHOD(AngelScriptComponentWrapper, release), asCALL_THISCALL);
	engine->RegisterObjectMethod("ScriptComponent", "const Object@ get_owner() const", asMETHODPR(AngelScriptComponentWrapper, getOwner, (void) const, const Object*), asCALL_THISCALL);
	engine->RegisterObjectMethod("ScriptComponent", "Object@ get_owner()", asMETHODPR(AngelScriptComponentWrapper, getOwner, (void), Object*), asCALL_THISCALL);
	//engine->RegisterObjectMethod("ScriptComponent", "const String@ get_name() const", asMETHOD(AngelScriptComponentWrapper, getName), asCALL_THISCALL);
	engine->RegisterObjectMethod("ScriptComponent", "bool get_active() const", asMETHOD(AngelScriptComponentWrapper, isActive), asCALL_THISCALL);
	engine->RegisterObjectMethod("ScriptComponent", "void set_active(bool)", asMETHOD(AngelScriptComponentWrapper, setActive), asCALL_THISCALL);

	engine->RegisterObjectMethod("ScriptComponent", "const Component@ opImplCast() const", asMETHODPR(AngelScriptComponentWrapper, impliedCast, (void) const, const Component*), asCALL_THISCALL);
	engine->RegisterObjectMethod("ScriptComponent", "Component@ opImplCast()", asMETHODPR(AngelScriptComponentWrapper, impliedCast, (void), Component*), asCALL_THISCALL);
}

const Component* AngelScriptComponentWrapper::impliedCast(void) const
{
	return _script_comp;
}

Component* AngelScriptComponentWrapper::impliedCast(void)
{
	return _script_comp;
}

const Object* AngelScriptComponentWrapper::getOwner(void) const
{
	GAFF_ASSERT(_script_comp);
	return _script_comp->getOwner();
}

Object* AngelScriptComponentWrapper::getOwner(void)
{
	GAFF_ASSERT(_script_comp);
	return _script_comp->getOwner();
}

bool AngelScriptComponentWrapper::isActive(void) const
{
	GAFF_ASSERT(_script_comp);
	//return _script_comp->getOwner()->isActive();
	return true;
}

void AngelScriptComponentWrapper::setActive(bool active)
{
	GAFF_REF(active);
	GAFF_ASSERT(_script_comp);
	//_script_comp->getOwner()->setActive(active);
}

NS_END
