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
#include "Gaff_Assert.h"

NS_GAFF

class ISerializeReader;
class ISerializeWriter;
class ISerializeInfo;

class IReflectionVar
{
public:
	virtual ~IReflectionVar(void) {}

	template <class DataType>
	void setDataT(void* object, const DataType& data)
	{
		GAFF_ASSERT(getType() == GetRVT< std::remove_reference<DataType>::type >());
		setData(object, &data);
	}

	template <class DataType>
	void setDataMoveT(void* object, DataType&& data)
	{
		GAFF_ASSERT(getType() == GetRVT< std::remove_reference<DataType>::type >());
		setDataMove(object, &data);
	}

	template <class DataType>
	const DataType& getElementT(const void* object, int32_t index) const
	{
		GAFF_ASSERT((isFixedArray() || isVector()) && size(object) > index);
		GAFF_ASSERT(getType() == GetRVT< std::remove_reference<DataType>::type >());
		return *reinterpret_cast<const DataType*>(getElement(object, index));
	}

	template <class DataType>
	void setElementT(void* object, int32_t index, const DataType& data)
	{
		GAFF_ASSERT((isFixedArray() || isVector()) && size(object) > index);
		GAFF_ASSERT(getType() == GetRVT< std::remove_reference<DataType>::type >());
		setElement(object, index, &data);
	}

	template <class DataType>
	void setElementMoveT(void* object, int32_t index, DataType&& data)
	{
		GAFF_ASSERT((isFixedArray() || isVector()) && size(object) > index);
		GAFF_ASSERT(getType() == GetRVT< std::remove_reference<DataType>::type >());
		setElementMove(object, index, &data);
	}

	virtual ReflectionValueType getType(void) const = 0;
	virtual const void* getData(const void* object) const = 0;
	virtual void setData(void* object, const void* data) = 0;
	virtual void setDataMove(void* object, void* data) = 0;

	virtual bool isFixedArray(void) const { return false; }
	virtual bool isVector(void) const { return false; }

	virtual int32_t size(const void*) const
	{
		GAFF_ASSERT_MSG(false, "Reflection variable is not an array or vector!");
		return 0;
	}

	virtual const void* getElement(const void*, int32_t) const
	{
		GAFF_ASSERT_MSG(false, "Reflection variable is not an array or vector!");
		return nullptr;
	}

	virtual void setElement(void*, int32_t, const void*)
	{
		GAFF_ASSERT_MSG(false, "Reflection variable is not an array or vector!");
	}

	virtual void setElementMove(void*, int32_t, void*)
	{
		GAFF_ASSERT_MSG(false, "Reflection variable is not an array or vector!");
	}

	virtual void swap(void*, int32_t, int32_t)
	{
		GAFF_ASSERT_MSG(false, "Reflection variable is not an array or vector!");
	}

	virtual void resize(void*, size_t)
	{
		GAFF_ASSERT_MSG(false, "Reflection variable is not a vector!");
	}
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
	virtual bool hasInterface(ReflectionHash class_hash) const = 0;

	virtual int32_t getNumVariables(void) const = 0;
	virtual Hash32 getVariableHash(int32_t index) const = 0;
	virtual IReflectionVar* getVariable(int32_t index) const = 0;
	virtual IReflectionVar* getVariable(Hash32 name) const = 0;
};

NS_END