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

#include "Gaff_AngelScriptModule.h"
#include "Gaff_AngelScriptFunction.h"

NS_GAFF

AngelScriptModule::AngelScriptModule(const AngelScriptModule& module):
	_module(module._module)
{
}

AngelScriptModule::AngelScriptModule(asIScriptModule* module):
	_module(module)
{
}

AngelScriptModule::~AngelScriptModule(void)
{
}

const AngelScriptModule& AngelScriptModule::operator=(const AngelScriptModule& rhs)
{
	_module = rhs._module;
	return *this;
}

const AngelScriptModule& AngelScriptModule::operator=(asIScriptModule* rhs)
{
	_module = rhs;
	return *this;
}

bool AngelScriptModule::operator==(const AngelScriptModule& rhs) const
{
	return _module == rhs._module;
}

bool AngelScriptModule::operator==(const asIScriptModule* rhs) const
{
	return _module == rhs;
}

bool AngelScriptModule::operator!=(const AngelScriptModule& rhs) const
{
	return _module != rhs._module;
}

bool AngelScriptModule::operator!=(const asIScriptModule* rhs) const
{
	return _module != rhs;
}

bool AngelScriptModule::isValid(void) const
{
	return _module != nullptr;
}

const char* AngelScriptModule::getName(void) const
{
	return _module->GetName();
}

unsigned int AngelScriptModule::getFunctionCount(void) const
{
	return _module->GetFunctionCount();
}

AngelScriptFunction AngelScriptModule::getFunction(unsigned int index) const
{
	return AngelScriptFunction(_module->GetFunctionByIndex(index));
}

AngelScriptFunction AngelScriptModule::getFunction(const char* name) const
{
	return AngelScriptFunction(_module->GetFunctionByName(name));
}

AngelScriptFunction AngelScriptModule::getFunctionByDeclaration(const char* declaration) const
{
	return AngelScriptFunction(_module->GetFunctionByDecl(declaration));
}

unsigned int AngelScriptModule::getGlobalVarCount(void) const
{
	return _module->GetGlobalVarCount();
}

int AngelScriptModule::getGlobalVarIndex(const char* name) const
{
	assert(name && strlen(name));
	return _module->GetGlobalVarIndexByName(name);
}

int AngelScriptModule::getGlobalVarIndexByDecl(const char* declaration) const
{
	assert(declaration && strlen(declaration));
	return _module->GetGlobalVarIndexByDecl(declaration);
}

const char* AngelScriptModule::getGlobalVarDeclaration(unsigned int index, bool include_namespace) const
{
	return _module->GetGlobalVarDeclaration(index, include_namespace);
}

bool AngelScriptModule::getGlobalVar(unsigned int index, const char** name, const char** name_space, /*int* type_id,*/ bool* is_const) const
{
	return _module->GetGlobalVar(index, name, name_space, nullptr, is_const) >= 0;
}

void* AngelScriptModule::getGlobalVarAddress(unsigned int index)
{
	return _module->GetAddressOfGlobalVar(index);
}

unsigned int AngelScriptModule::getObjectTypeCount(void) const
{
	return _module->GetObjectTypeCount();
}

//AngelScriptModule::getObjectType(unsigned int index) const
//{
//	return _module->GetObjectTypeByIndex(index);
//}
//
//AngelScriptModule::getObjectType(const char* name) const
//{
//	return _module->GetObjectTypeByName(name);
//}

int AngelScriptModule::getTypeID(const char* declaration) const
{
	assert(declaration && strlen(declaration));
	return _module->GetTypeIdByDecl(declaration);
}

unsigned int AngelScriptModule::getEnumCount(void) const
{
	return _module->GetEnumCount();
}

const char* AngelScriptModule::getEnum(unsigned int index, int* enum_type_id, const char** name_space) const
{
	return _module->GetEnumByIndex(index, enum_type_id, name_space);
}

int AngelScriptModule::getEnumValueCount(unsigned int enum_type_id) const
{
	return _module->GetEnumValueCount(enum_type_id);
}

const char* AngelScriptModule::getEnumValue(int enum_type_id, unsigned int index, int* value) const
{
	return _module->GetEnumValueByIndex(enum_type_id, index, value);
}

unsigned int AngelScriptModule::getTypedefCount(void) const
{
	return _module->GetTypedefCount();
}

const char* AngelScriptModule::getTypeDef(unsigned int index, int* type_id, const char** name_space) const
{
	return _module->GetTypedefByIndex(index, type_id, name_space);
}

NS_END
