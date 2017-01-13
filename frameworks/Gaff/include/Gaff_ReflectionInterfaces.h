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
#include "Gaff_Vector.h"
#include "Gaff_Assert.h"

NS_GAFF

class ISerializeReader;
class ISerializeWriter;
class IAllocator;

class IReflectionDefinition;

class IReflectionObject
{
public:
	virtual ~IReflectionObject(void) {}

	virtual const IReflectionDefinition& getReflectionDefinition(void) const = 0;

	virtual const void* getBasePointer(void) const = 0;
	virtual void* getBasePointer(void) = 0;
};

class IAttribute : public IReflectionObject
{
};

class IReflection
{
public:
	virtual ~IReflection(void) {}

	virtual void init(void) = 0;

	virtual void load(ISerializeReader& reader, void* object) const = 0;
	virtual void save(ISerializeWriter& writer, const void* object) const = 0;
	virtual const char* getName(void) const = 0;
	virtual Hash64 getHash(void) const = 0;
	virtual Hash64 getVersion(void) const = 0;

	IReflection* attr_next = nullptr;
	IReflection* next = nullptr;
};

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
	template <class... Args>
	using FactoryFunc = void* (*)(IAllocator&, Args&&...);

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

	template <class T, class... Args>
	T* createAllocT(IAllocator& allocator, Args&&... args) const
	{
		Hash64 hash = getReflectionInstance().getHash();
		GAFF_ASSERT(hasInterface(hash));

		Hash64 ctor_hash = CalcTemplateHash<Args...>(INIT_HASH64);

		FactoryFunc<Args...> factory_func = reinterpret_cast< FactoryFunc<Args...> >(getFactory(ctor_hash));

		T* instance = nullptr;

		if (factory_func) {
			void* data = factory_func(allocator, std::forward<Args>(args)...);
			instance = reinterpret_cast<T*>(getInterface(hash, data));
		}

		return instance;
	}

	template <class... Args>
	void* createAlloc(IAllocator& allocator, Args&&... args) const
	{
		Hash64 ctor_hash = CalcTemplateHash<Args...>(INIT_HASH64);

		FactoryFunc<Args...> factory_func = reinterpret_cast< FactoryFunc<Args...> >(getFactory(ctor_hash));

		if (factory_func) {
			return factory_func(allocator, std::forward<Args>(args)...);
		}

		return nullptr;
	}

	template <class T>
	const T* getClassAttribute(void) const
	{
		for (int32_t i = 0; i < getNumClassAttributes(); ++i) {
			const IAttribute* const attribute = getClassAttribute(i);
			const T* attr = ReflectionCast<T>(*attribute);

			if (attr) {
				return attr;
			}
		}

		return nullptr;
	}

	template <class T>
	const T* getVarAttribute(Hash32 name) const
	{
		const int32_t size = getNumVarAttributes(name);

		for (int32_t i = 0; i < size; ++i) {
			const IAttribute* const attribute = getVarAttribute(name, i);
			const T* attr = ReflectionCast<T>(*attribute);

			if (attr) {
				return attr;
			}
		}

		return nullptr;
	}

	template <class T, class Allocator>
	void getClassAttributes(Vector<const T*, Allocator>& out) const
	{
		for (int32_t i = 0; i < getNumClassAttributes(); ++i) {
			const IAttribute* const attribute = getClassAttribute(i);
			const T* attr = ReflectionCast<T>(*attribute);

			if (attr) {
				out.emplace_back(attr);
			}
		}
	}

	template <class T, class Allocator>
	void getVarAttributes(Vector<const T*, Allocator>& out) const
	{
		const int32_t size = getNumVarAttributes(name);

		for (int32_t i = 0; i < size; ++i) {
			const IAttribute* const attribute = getVarAttribute(name, i);
			const T* attr = ReflectionCast<T>(*attribute);

			if (attr) {
				out.emplace_back(attr);
			}
		}
	}

	template <class... Args>
	FactoryFunc<Args...> getFactory(void) const
	{
		Hash64 ctor_hash = Gaff::CalcTemplateHash<Args...>(INIT_HASH64);
		return reinterpret_cast< FactoryFunc<Args...> >(getFactory(ctor_hash));
	}

	virtual ~IReflectionDefinition(void) {}

	virtual const IReflection& getReflectionInstance(void) const = 0;

	virtual void load(ISerializeReader& reader, void* object) const = 0;
	virtual void save(ISerializeWriter& writer, const void* object) const = 0;

	virtual const void* getInterface(Hash64 class_id, const void* object) const = 0;
	virtual void* getInterface(Hash64 class_id, void* object) const = 0;
	virtual bool hasInterface(Hash64 class_hash) const = 0;

	virtual int32_t getNumVariables(void) const = 0;
	virtual Hash32 getVariableHash(int32_t index) const = 0;
	virtual IReflectionVar* getVariable(int32_t index) const = 0;
	virtual IReflectionVar* getVariable(Hash32 name) const = 0;

	virtual int32_t getNumClassAttributes(void) const = 0;
	virtual const IAttribute* getClassAttribute(int32_t index) const = 0;

	virtual int32_t getNumVarAttributes(Hash32 name) const = 0;
	virtual const IAttribute* getVarAttribute(Hash32 name, int32_t index) const = 0;

	using VoidFunc = void (*)(void);

	virtual VoidFunc getFactory(Hash64 ctor_hash) const = 0;
};

NS_END
