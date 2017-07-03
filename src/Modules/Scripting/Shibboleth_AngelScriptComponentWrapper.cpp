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
#include <Shibboleth_IncludeAngelScript.h>

NS_SHIBBOLETH

static void* WrapperFactory(void)
{
	IAllocator* const allocator = GetAllocator();
	int32_t pool_index = allocator->getPoolIndex("AngelScript");

	AngelScriptComponentWrapper* const instance = SHIB_ALLOCT_POOL(AngelScriptComponentWrapper, pool_index, *allocator);
	instance->addRef();
	return instance;
}

static void GetInterface(asIScriptGeneric* generic)
{
	AngelScriptComponentWrapper* const wrapper = reinterpret_cast<AngelScriptComponentWrapper*>(generic->GetObject());
	void** const out = reinterpret_cast<void**>(generic->GetArgAddress(0));
	const int type_id = generic->GetArgTypeId(0);

	GAFF_ASSERT(type_id > asTYPEID_DOUBLE);
	GAFF_ASSERT(type_id & asTYPEID_OBJHANDLE);

	asITypeInfo* const type_info = generic->GetEngine()->GetTypeInfoById(type_id);
	asIScriptObject* const script_object = wrapper->getScript()->getObject();
	asITypeInfo* const script_type_info = script_object->GetObjectType();

	script_object->GetEngine()->RefCastObject(script_object, script_type_info, type_info, out);
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

	engine->RegisterObjectMethod("ScriptComponent", "void opCast(?& out)", asFUNCTION(GetInterface), asCALL_GENERIC);
}

const AngelScriptComponent* AngelScriptComponentWrapper::getScript(void) const
{
	return _script_comp;
}

AngelScriptComponent* AngelScriptComponentWrapper::getScript(void)
{
	return _script_comp;
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
