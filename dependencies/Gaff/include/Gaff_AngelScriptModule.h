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

#include "Gaff_String.h"
#include "Gaff_AngelScriptFunction.h"

NS_GAFF

class AngelScriptModule
{
public:
	template <class Allocator>
	INLINE AngelScriptFunction getFunction(const Gaff::String<char, Allocator>& declaration)
	{
		assert(declaration.size());
		return getFunction(declaration.getBuffer());
	}

	template <class Allocator>
	INLINE AngelScriptFunction getFunctionByName(const Gaff::String<char, Allocator>& name)
	{
		assert(name.size());
		return getFunctionByName(name.getBuffer());
	}

	template <class Allocator>
	INLINE int getGlobalVarIndex(const Gaff::String<char, Allocator>& name)
	{
		assert(name.size());
		return getGlobalVarIndex(name.getBuffer());
	}

	template <class Allocator>
	INLINE int getGlobalVarIndexByDecl(const Gaff::String<char, Allocator>& declaration)
	{
		assert(declaration.size());
		return getGlobalVarIndexByDecl(declaration.getBuffer());
	}

	AngelScriptModule(const AngelScriptModule& module);
	explicit AngelScriptModule(asIScriptModule* module);
	~AngelScriptModule(void);

	const AngelScriptModule& operator=(const AngelScriptModule& rhs);
	const AngelScriptModule& operator=(asIScriptModule* rhs);

	bool operator==(const AngelScriptModule& rhs) const;
	bool operator==(const asIScriptModule* rhs) const;
	bool operator!=(const AngelScriptModule& rhs) const;
	bool operator!=(const asIScriptModule* rhs) const;

	bool isValid(void) const;
	const char* getName(void) const;

	unsigned int getFunctionCount(void) const;
	AngelScriptFunction getFunction(unsigned int index) const;
	AngelScriptFunction getFunction(const char* name) const;
	AngelScriptFunction getFunctionByDeclaration(const char* declaration) const;

	unsigned int getGlobalVarCount(void) const;
	int getGlobalVarIndex(const char* name) const;
	int getGlobalVarIndexByDecl(const char* declaration) const;
	const char* getGlobalVarDeclaration(unsigned int index, bool include_namespace = false) const;
	bool getGlobalVar(unsigned int index, const char** name, const char** name_space = nullptr, /*int* type_id = nullptr,*/ bool* is_const = nullptr) const;
	void* getGlobalVarAddress(unsigned int index);

	unsigned int getObjectTypeCount(void) const;
	//getObjectType(unsigned int index) const;
	//getObjectType(const char* name) const;
	int getTypeID(const char* declaration) const;

	unsigned int getEnumCount(void) const;
	const char* getEnum(unsigned int index, int* enum_type_id, const char** name_space = nullptr) const;
	int getEnumValueCount(unsigned int enum_type_id) const;
	const char* getEnumValue(int enum_type_id, unsigned int index, int* value) const;

	unsigned int getTypedefCount(void) const;
	const char* getTypeDef(unsigned int index, int* type_id, const char** name_space = nullptr) const;

private:
	asIScriptModule* _module;
};

NS_END
