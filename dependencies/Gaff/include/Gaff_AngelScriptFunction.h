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

#include "Gaff_RefPtr.h"
#include "Gaff_IncludeAngelScript.h"

NS_GAFF

class AngelScriptModule;

class AngelScriptFunction
{
public:
	AngelScriptFunction(const AngelScriptFunction& rhs);
	explicit AngelScriptFunction(asIScriptFunction* function);
	~AngelScriptFunction(void);

	INLINE const AngelScriptFunction& operator=(const AngelScriptFunction& rhs);
	INLINE const AngelScriptFunction& operator=(asIScriptFunction* rhs);

	INLINE bool operator==(const AngelScriptFunction& rhs) const;
	INLINE bool operator==(const asIScriptFunction* rhs) const;
	INLINE bool operator!=(const AngelScriptFunction& rhs) const;
	INLINE bool operator!=(const asIScriptFunction* rhs) const;

	INLINE bool isValid(void) const;

	INLINE int getID(void) const;
	INLINE const char* getModuleName(void) const;
	INLINE AngelScriptModule getModule(void) const;
	INLINE const char* getScriptSectionName(void) const;

	INLINE const char* getObjectName(void) const;
	INLINE const char* getName(void) const;
	INLINE const char* getNamespace(void) const;
	INLINE const char* getDeclaration(bool include_obj_name = true, bool include_namespace = false) const;
	INLINE bool isReadOnly(void) const;
	INLINE bool isPrivate(void) const;
	INLINE bool isFinal(void) const;
	INLINE bool isOverride(void) const;
	INLINE bool isShared(void) const;
	INLINE unsigned int getParamCount(void) const;
	INLINE int getParamTypeID(unsigned int index) const;
	INLINE int getReturnTypeID(void) const;

private:
	COMRefPtr<asIScriptFunction> _function;

	friend class AngelScriptContext;
};

NS_END
