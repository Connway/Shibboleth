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

#ifdef PLATFORM_WINDOWS
	#pragma warning(push)
	#pragma warning(disable : 4505)
#endif

NS_GAFF

template <class Allocator>
using ReflectionHashString = HashString<char, ReflectionHash, Allocator>;

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
			GAFF_ASSERT(getType() == GetRVT< std::remove_reference<DataType>::type >());
			return *reinterpret_cast<const DataType*>(getData(&object));
		}

		template <class DataType>
		void setDataT(T& object, const DataType& data)
		{
			GAFF_ASSERT(getType() == GetRVT< std::remove_reference<DataType>::type >());
			setData(&object, &data);
		}

		template <class DataType>
		void setDataMoveT(T& object, DataType&& data)
		{
			GAFF_ASSERT(getType() == GetRVT< std::remove_reference<DataType>::type >());
			setDataMove(&object, &data);
		}

		template <class DataType>
		const DataType& getElementT(const T& object, int32_t index) const
		{
			GAFF_ASSERT((isFixedArray() || isVector()) && size(&object) > index);
			GAFF_ASSERT(getType() == GetRVT< std::remove_reference<DataType>::type >());
			return *reinterpret_cast<const DataType*>(getElement(&object, index));
		}

		template <class DataType>
		void setElementT(T& object, int32_t index, const DataType& data)
		{
			GAFF_ASSERT((isFixedArray() || isVector()) && size(&object) > index);
			GAFF_ASSERT(getType() == GetRVT< std::remove_reference<DataType>::type >());
			setElement(&object, index, &data);
		}

		template <class DataType>
		void setElementMoveT(T& object, int32_t index, DataType&& data)
		{
			GAFF_ASSERT((isFixedArray() || isVector()) && size(&object) > index);
			GAFF_ASSERT(getType() == GetRVT< std::remove_reference<DataType>::type >());
			setElementMove(&object, index, &data);
		}

		virtual ~IVar(void) {}
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
	bool hasInterface(ReflectionHash class_hash) const override;

	void setAllocator(const Allocator& allocator);

	const ISerializeInfo& getReflectionInstance(void) const override;

	Hash64 getVersionHash(void) const;

	int32_t getNumVariables(void) const override;
	ReflectionHash getVariableHash(int32_t index) const override;
	IReflectionVar* getVariable(int32_t index) const override;
	IReflectionVar* getVariable(ReflectionHash name) const override;

	const ReflectionHashString<Allocator>& getVariableName(int32_t index) const;
	IVar* getVar(int32_t index) const;
	IVar* getVar(ReflectionHash name) const;

	template <class Base>
	ReflectionDefinition& base(const char* name, ReflectionHash hash);

	template <class Base>
	ReflectionDefinition& base(void);

	template <class... Args>
	ReflectionDefinition& ctor(void);

	template <class Var, size_t size>
	ReflectionDefinition& var(const char (&name)[size], Var T::*ptr);

	template <class Ret, class Var, size_t size>
	ReflectionDefinition& var(const char (&name)[size], Ret (T::*getter)(void) const, void (T::*setter)(Var));

	template <class Var, class Vec_Allocator, size_t size>
	ReflectionDefinition& var(const char (&name)[size], Vector<Var, Vec_Allocator> T::*vec);

	template <class Var, size_t array_size, size_t name_size>
	ReflectionDefinition& var(const char (&name)[name_size], Var (T::*arr)[array_size]);

	template <size_t size, class Ret, class... Args>
	ReflectionDefinition& func(const char (&name)[size], Ret (T::*ptr)(Args...) const);

	template <size_t size, class Ret, class... Args>
	ReflectionDefinition& func(const char (&name)[size], Ret (T::*ptr)(Args...));

	void finish(void);

private:
	template <class Var>
	class VarPtr final : public IVar
	{
	public:
		VarPtr(Var T::*ptr);

		ReflectionValueType getType(void) const override;
		const void* getData(const void* object) const override;
		void setData(void* object, const void* data) override;
		void setDataMove(void* object, void* data) override;

	private:
		Var T::*_ptr = nullptr;
	};

	template <class Ret, class Var>
	class VarFuncPtr final : public IVar
	{
	public:
		using Getter = Ret (T::*)(void) const;
		using Setter = void (T::*)(Var);

		VarFuncPtr(Getter getter, Setter setter);

		ReflectionValueType getType(void) const override;
		const void* getData(const void* object) const override;
		void setData(void* object, const void* data) override;
		void setDataMove(void* object, void* data) override;

	private:
		Getter _getter = nullptr;
		Setter _setter = nullptr;

		mutable const typename std::remove_reference<Ret>::type* _copy_ptr = nullptr;
		mutable typename std::remove_const< typename std::remove_reference<Ret>::type >::type _copy;
	};

	template <class Base>
	class BaseVarPtr final : public IVar
	{
	public:
		BaseVarPtr(typename ReflectionDefinition<Base, Allocator>::IVar* base_var);

		ReflectionValueType getType(void) const override;
		const void* getData(const void* object) const override;
		void setData(void* object, const void* data) override;
		void setDataMove(void* object, void* data) override;

		bool isFixedArray(void) const override;
		bool isVector(void) const override;
		int32_t size(const void*) const override;

		const void* getElement(const void* object, int32_t index) const override;
		void setElement(void* object, int32_t index, const void* data) override;
		void setElementMove(void* object, int32_t index, void* data) override;
		void swap(void* object, int32_t index_a, int32_t index_b) override;
		void resize(void* object, size_t new_size) override;

	private:
		typename ReflectionDefinition<Base, Allocator>::IVar* _base_var;
	};

	template <class Var, size_t array_size>
	class ArrayPtr final : public IVar
	{
	public:
		ArrayPtr(Var (T::*ptr)[array_size]);

		ReflectionValueType getType(void) const override;
		const void* getData(const void* object) const override;
		void setData(void* object, const void* data) override;
		void setDataMove(void* object, void* data) override;

		bool isFixedArray(void) const override { return true; }
		bool isVector(void) const override { return false; }
		int32_t size(const void*) const override { return static_cast<int32_t>(array_size); }

		const void* getElement(const void* object, int32_t index) const override;
		void setElement(void* object, int32_t index, const void* data) override;
		void setElementMove(void* object, int32_t index, void* data) override;
		void swap(void* object, int32_t index_a, int32_t index_b) override;
		void resize(void* object, size_t new_size) override;
	
	private:
		Var (T::*_ptr)[array_size] = nullptr;
	};

	template <class Var, class Vec_Allocator>
	class VectorPtr final : public IVar
	{
	public:
		VectorPtr(Vector<Var, Vec_Allocator> T::*ptr);

		ReflectionValueType getType(void) const override;
		const void* getData(const void* object) const override;
		void setData(void* object, const void* data) override;
		void setDataMove(void* object, void* data) override;

		bool isFixedArray(void) const override { return false; }
		bool isVector(void) const override { return true; }
		int32_t size(const void* object) const override;

		const void* getElement(const void* object, int32_t index) const override;
		void setElement(void* object, int32_t index, const void* data) override;
		void setElementMove(void* object, int32_t index, void* data) override;
		void swap(void* object, int32_t index_a, int32_t index_b) override;
		void resize(void* object, size_t new_size) override;

	private:
		Vector<Var, Vec_Allocator> T::*_ptr = nullptr;
	};

	using IVarPtr = UniquePtr<IVar, Allocator>;

	VectorMap<ReflectionHashString<Allocator>, ptrdiff_t, Allocator> _base_class_offsets;
	VectorMap<ReflectionHashString<Allocator>, IVarPtr, Allocator> _vars;

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

#define BASE(type) base<type>(#type, REFL_HASH_CONST(#type))

#ifdef PLATFORM_WINDOWS
	#pragma warning(pop)
#endif
