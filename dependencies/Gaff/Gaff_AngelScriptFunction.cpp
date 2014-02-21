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

#include "Gaff_AngelScriptFunction.h"
#include "Gaff_AngelScriptModule.h"

NS_GAFF

AngelScriptFunction::AngelScriptFunction(const AngelScriptFunction& rhs):
	_function(rhs._function)
{
}


AngelScriptFunction::AngelScriptFunction(asIScriptFunction* function):
	_function(function)
{
}

AngelScriptFunction::~AngelScriptFunction(void)
{
}

const AngelScriptFunction& AngelScriptFunction::operator=(const AngelScriptFunction& rhs)
{
	_function = rhs._function;
	return *this;
}

const AngelScriptFunction& AngelScriptFunction::operator=(asIScriptFunction* rhs)
{
	_function = rhs;
	return *this;
}

bool AngelScriptFunction::operator==(const AngelScriptFunction& rhs) const
{
	return _function == rhs._function;
}

bool AngelScriptFunction::operator==(const asIScriptFunction* rhs) const
{
	return _function == rhs;
}

bool AngelScriptFunction::operator!=(const AngelScriptFunction& rhs) const
{
	return _function != rhs._function;
}

bool AngelScriptFunction::operator!=(const asIScriptFunction* rhs) const
{
	return _function != rhs;
}

bool AngelScriptFunction::isValid(void) const
{
	return _function != nullptr;
}

int AngelScriptFunction::getID(void) const
{
	return _function->GetId();
}

const char* AngelScriptFunction::getModuleName(void) const
{
	return _function->GetModuleName();
}

AngelScriptModule AngelScriptFunction::getModule(void) const
{
	return AngelScriptModule(_function->GetModule());
}

const char* AngelScriptFunction::getScriptSectionName(void) const
{
	return _function->GetScriptSectionName();
}

const char* AngelScriptFunction::getObjectName(void) const
{
	return _function->GetObjectName();
}

const char* AngelScriptFunction::getName(void) const
{
	return _function->GetName();
}

const char* AngelScriptFunction::getNamespace(void) const
{
	return _function->GetNamespace();
}

const char* AngelScriptFunction::getDeclaration(bool include_obj_name, bool include_namespace) const
{
	return _function->GetDeclaration(include_obj_name, include_namespace);
}

bool AngelScriptFunction::isReadOnly(void) const
{
	return _function->IsReadOnly();
}

bool AngelScriptFunction::isPrivate(void) const
{
	return _function->IsPrivate();
}

bool AngelScriptFunction::isFinal(void) const
{
	return _function->IsFinal();
}

bool AngelScriptFunction::isOverride(void) const
{
	return _function->IsOverride();
}

bool AngelScriptFunction::isShared(void) const
{
	return _function->IsShared();
}

unsigned int AngelScriptFunction::getParamCount(void) const
{
	return _function->GetParamCount();
}

int AngelScriptFunction::getParamTypeID(unsigned int index) const
{
	return _function->GetParamTypeId(index);
}

int AngelScriptFunction::getReturnTypeID(void) const
{
	return _function->GetReturnTypeId();
}

NS_END
