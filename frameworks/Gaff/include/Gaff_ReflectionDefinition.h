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

#include "Gaff_IReflectionDefinition.h"
#include "Gaff_ReflectionVersion.h"
#include "Gaff_HashString.h"
#include "Gaff_VectorMap.h"
#include "Gaff_Assert.h"
#include "Gaff_Utils.h"

NS_GAFF

template <class T, class Allocator>
class ReflectionDefinition final : public IReflectionDefinition
{
public:
	class IVar : public IReflectionVar
	{
	public:
		template <class DataType>
		const DataType& getDataT(const T& object) const
		{
			GAFF_ASSERT(getType() == GetRVT<DataType>());
			return *reinterpret_cast<const DataType*>(getData(object));
		}

		template <class DataType>
		void setDataT(T& object, const DataType& data)
		{
			GAFF_ASSERT(getType() == GetRVT<DataType>());
			setData(object, &data);
		}

		template <class DataType>
		void setDataT(T& object, DataType&& data)
		{
			GAFF_ASSERT(getType() == GetRVT<DataType>());
			setDataMove(object, &data);
		}

		virtual ~IVar(void) {}

		virtual ReflectionValueType getType(void) const = 0;
		virtual const void* getData(const T& object) const = 0;
		virtual void setData(T& object, const void* data) = 0; // asignment copy
		virtual void setDataMove(T& object, void* data) = 0; // assignment move if possible

		const void* getData(const void* object) const override
		{
			GAFF_ASSERT(object);
			return getData(*reinterpret_cast<const T*>(object));
		}

		void setData(void* object, const void* data) override
		{
			GAFF_ASSERT(object);
			setData(*reinterpret_cast<T*>(object), data);
		}

		void setDataMove(void* object, void* data) override
		{
			GAFF_ASSERT(object);
			setData(*reinterpret_cast<T*>(object), data);
		}
	};

	GAFF_STRUCTORS_DEFAULT(ReflectionDefinition);
	GAFF_NO_COPY(ReflectionDefinition);
	GAFF_NO_MOVE(ReflectionDefinition);

	void load(ISerializeReader& reader, void* object) const override;
	void save(ISerializeWriter& writer, const void* object) const override;
	void load(ISerializeReader& reader, T& object) const;
	void save(ISerializeWriter& writer, const T& object) const;

	const void* getInterface(ReflectionHash class_hash, const void* object) const override;
	void* getInterface(ReflectionHash class_hash, void* object) const override;

	void setAllocator(const Allocator& allocator);

	void setReflectionInstance(const ISerializeInfo& reflection_instance);
	const ISerializeInfo& getReflectionInstance(void) const override;

	Hash64 getVersionHash(void) const;

	int32_t getNumVariables(void) const override;
	Hash32 getVariableHash(int32_t index) const override;
	IReflectionVar* getVariable(int32_t index) const override;
	IReflectionVar* getVariable(Hash32 name) const override;

	const HashString32<Allocator>& getVariableName(int32_t index) const;
	IVar* getVar(int32_t index) const;
	IVar* getVar(Hash32 name) const;

	ReflectionDefinition& baseClass(const char* name, ReflectionHash hash, ptrdiff_t offset);

	template <class Base>
	ReflectionDefinition& baseClass(void);

	template <class Var, size_t size>
	ReflectionDefinition& var(const char(&name)[size], Var T::*ptr);

	template <class Ret, class Var, size_t size>
	ReflectionDefinition& var(const char(&name)[size], Ret (T::*getter)(void) const, void (T::*setter)(Var));

	//template <class Var, class Vec_Allocator, size_t size>
	//ReflectionDefinition& var(const char(&name)[size], Vector<Var, Vec_Allocator> T::*vec);

	//template <class Var, size_t array_size, size_t name_size>
	//ReflectionDefinition& var(const char(&name)[name_size], Var (T::*arr)[array_size]);

	void finish(void);

private:
	template <class Var>
	class VarPtr final : public IVar
	{
	public:
		VarPtr(Var T::*ptr);

		ReflectionValueType getType(void) const override;
		const void* getData(const T& object) const override;
		void setData(T& object, const void* data) override;
		void setDataMove(T& object, void* data) override;

	private:
		Var T::*_ptr;
	};

	template <class Ret, class Var>
	class VarFuncPtr final : public IVar
	{
	public:
		using Getter = Ret (T::*)(void) const;
		using Setter = void (T::*)(Var);

		VarFuncPtr(Getter getter, Setter setter);

		ReflectionValueType getType(void) const override;
		const void* getData(const T& object) const override;
		void setData(T& object, const void* data) override;
		void setDataMove(T& object, void* data) override;

	private:
		Getter _getter;
		Setter _setter;

		mutable const typename std::remove_reference<Ret>::type* _copy_ptr = nullptr;
		mutable typename std::remove_const< typename std::remove_reference<Ret>::type >::type _copy;
	};

	template <class Base>
	class BaseVarPtr final : public IVar
	{
	public:
		BaseVarPtr(typename ReflectionDefinition<Base, Allocator>::IVar* base_var);

		ReflectionValueType getType(void) const override;
		const void* getData(const T& object) const override;
		void setData(T& object, const void* data) override;
		void setDataMove(T& object, void* data) override;

	private:
		typename ReflectionDefinition<Base, Allocator>::IVar* _base_var;
	};

	using IVarPtr = UniquePtr<IVar, Allocator>;

	VectorMap<HashString32<Allocator>, ptrdiff_t, Allocator> _base_class_offsets;
	VectorMap<HashString32<Allocator>, IVarPtr, Allocator> _vars;

	const ISerializeInfo* _reflection_instance = nullptr;
	Allocator _allocator;

	int32_t _base_classes_remaining = 0;

	ReflectionVersion<T> _version;

	template <class Base>
	static void RegisterBaseVariables(void);

	template <class RefT, class Allocator>
	friend class ReflectionDefinition;
};

NS_END

#include "Gaff_ReflectionDefinition.inl"

#define BASE_CLASS(type) baseClass(#type, REFL_HASH_CONST(#type), Gaff::OffsetOfClass<ThisType, type>())
