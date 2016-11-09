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

#include "Gaff_Reflection.h"

NS_GAFF

class ISerializeReader;
class ISerializeWriter;
class ISerializeInfo;

class IReflectionVar
{
public:
	virtual ~IReflectionVar(void) {}

	virtual const void* getData(const void* object) const = 0;
	virtual void setData(void* object, const void* data) = 0;
	virtual void setDataMove(void* object, void* data) = 0;
};

class IReflectionDefinition
{
public:
	template <class T>
	const T* getInterface(const void* object) const
	{
		return reinterpret_cast<const T*>(getInterface(T::GetReflectionHash(), object));
	}

	template <class T>
	T* getInterface(void* object)
	{
		return reinterpret_cast<T*>(getInterface(T::GetReflectionHash(), object));
	}

	virtual ~IReflectionDefinition(void) {}

	virtual const ISerializeInfo& getReflectionInstance(void) const = 0;

	virtual void load(ISerializeReader& reader, void* object) const = 0;
	virtual void save(ISerializeWriter& writer, const void* object) const = 0;

	virtual const void* getInterface(ReflectionHash class_id, const void* object) const = 0;
	virtual void* getInterface(ReflectionHash class_id, void* object) const = 0;

	virtual int32_t getNumVariables(void) const = 0;
	virtual Hash32 getVariableHash(int32_t index) const = 0;
	virtual IReflectionVar* getVariable(int32_t index) const = 0;
	virtual IReflectionVar* getVariable(Hash32 name) const = 0;
};

NS_END
