/************************************************************************************
Copyright (C) 2018 by Nicholas LaCroix

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

#include "Gaff_ReflectionInterfaces.h"
#include "Gaff_HashString.h"
#include "Gaff_VectorMap.h"
#include "Gaff_Assert.h"
#include "Gaff_Utils.h"

#ifdef PLATFORM_WINDOWS
	#pragma warning(push)
	#pragma warning(disable : 4505)
#endif

NS_GAFF

template <class T, class Allocator>
class ReflectionDefinition final : public IReflectionDefinition
{
public:
	using IAttributePtr = UniquePtr<IAttribute, Allocator>;

	class IVar : public IReflectionVar
	{
	public:
		virtual ~IVar(void) {}

		template <class DataType>
		const DataType& getDataT(const T& object) const;

		template <class DataType>
		void setDataT(T& object, const DataType& data);

		template <class DataType>
		void setDataMoveT(T& object, DataType&& data);

		template <class DataType>
		const DataType& getElementT(const T& object, int32_t index) const;

		template <class DataType>
		void setElementT(T& object, int32_t index, const DataType& data);

		template <class DataType>
		void setElementMoveT(T& object, int32_t index, DataType&& data);

		virtual void load(const ISerializeReader& reader, T& object) = 0;
		virtual void save(ISerializeWriter& writer, const T& object) = 0;
	};

	GAFF_STRUCTORS_DEFAULT(ReflectionDefinition);
	GAFF_NO_COPY(ReflectionDefinition);
	GAFF_NO_MOVE(ReflectionDefinition);

	template <class... Args>
	T* create(Args&&... args) const;

	void load(const ISerializeReader& reader, void* object) const override;
	void save(ISerializeWriter& writer, const void* object) const override;
	void load(const ISerializeReader& reader, T& object) const;
	void save(ISerializeWriter& writer, const T& object) const;

	const void* getInterface(Hash64 class_hash, const void* object) const override;
	void* getInterface(Hash64 class_hash, void* object) const override;
	bool hasInterface(Hash64 class_hash) const override;

	void setAllocator(const Allocator& allocator);

	const IReflection& getReflectionInstance(void) const override;

	int32_t getNumVariables(void) const override;
	Hash32 getVariableHash(int32_t index) const override;
	IReflectionVar* getVariable(int32_t index) const override;
	IReflectionVar* getVariable(Hash32 name) const override;

	int32_t getNumClassAttributes(void) const override;
	const IAttribute* getClassAttribute(int32_t index) const override;

	int32_t getNumVarAttributes(Hash32 name) const override;
	const IAttribute* getVarAttribute(Hash32 name, int32_t index) const override;

	int32_t getNumFuncAttributes(Hash32 name) const override;
	const IAttribute* getFuncAttribute(Hash32 name, int32_t index) const override;

	VoidFunc getFactory(Hash64 ctor_hash) const override;
	void* getFunc(Hash32 name, Hash64 args) const override;

	const HashString32<Allocator>& getVariableName(int32_t index) const;
	IVar* getVar(int32_t index) const;
	IVar* getVar(Hash32 name) const;

	template <class Base>
	ReflectionDefinition& base(const char* name);

	template <class Base>
	ReflectionDefinition& base(void);

	template <class... Args>
	ReflectionDefinition& ctor(void);

	template <class Var, size_t size>
	ReflectionDefinition& var(const char (&name)[size], Var T::*ptr, bool read_only = false);

	template <class Ret, class Var, size_t size>
	ReflectionDefinition& var(const char (&name)[size], Ret (T::*getter)(void) const, void (T::*setter)(Var));

	template <class Var, class Vec_Allocator, size_t size>
	ReflectionDefinition& var(const char (&name)[size], Vector<Var, Vec_Allocator> T::*vec, bool read_only = false);

	template <class Var, size_t array_size, size_t name_size>
	ReflectionDefinition& var(const char (&name)[name_size], Var (T::*arr)[array_size], bool read_only = false);

	template <size_t size, class Ret, class... Args>
	ReflectionDefinition& func(const char (&name)[size], Ret (T::*ptr)(Args...) const);

	template <size_t size, class Ret, class... Args>
	ReflectionDefinition& func(const char (&name)[size], Ret (T::*ptr)(Args...));

	template <class... Args>
	ReflectionDefinition& classAttrs(const Args&... args);

	template <size_t size, class... Args>
	ReflectionDefinition& varAttrs(const char (&name)[size], const Args&... args);

	template <size_t size, class... Args>
	ReflectionDefinition& funcAttrs(const char (&name)[size], const Args&... args);

	ReflectionDefinition& attrFile(const char* file);

	void finish(void);

private:
	template <class Var>
	class VarPtr final : public IVar
	{
	public:
		VarPtr(Var T::*ptr, bool read_only);

		ReflectionValueType getType(void) const override;
		const void* getData(const void* object) const override;
		void setData(void* object, const void* data) override;
		void setDataMove(void* object, void* data) override;

		bool isReadOnly(void) const override;

		void load(const ISerializeReader& reader, T& object) override;
		void save(ISerializeWriter& writer, const T& object) override;

	private:
		Var T::*_ptr = nullptr;
		const bool _read_only = false;
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

		bool isReadOnly(void) const override;

		void load(const ISerializeReader& reader, T& object) override;
		void save(ISerializeWriter& writer, const T& object) override;

	private:
		Getter _getter = nullptr;
		Setter _setter = nullptr;

		union
		{
			mutable const typename std::remove_reference<Ret>::type* _copy_ptr = nullptr;
			mutable typename std::remove_const< typename std::remove_reference<Ret>::type >::type _copy;
		};
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
		bool isReadOnly(void) const override;
		int32_t size(const void*) const override;

		const void* getElement(const void* object, int32_t index) const override;
		void setElement(void* object, int32_t index, const void* data) override;
		void setElementMove(void* object, int32_t index, void* data) override;
		void swap(void* object, int32_t index_a, int32_t index_b) override;
		void resize(void* object, size_t new_size) override;

		void load(const ISerializeReader& reader, T& object) override;
		void save(ISerializeWriter& writer, const T& object) override;

	private:
		typename ReflectionDefinition<Base, Allocator>::IVar* _base_var;
	};

	template <class Var, size_t array_size>
	class ArrayPtr final : public IVar
	{
	public:
		ArrayPtr(Var (T::*ptr)[array_size], bool read_only);

		ReflectionValueType getType(void) const override;
		const void* getData(const void* object) const override;
		void setData(void* object, const void* data) override;
		void setDataMove(void* object, void* data) override;

		bool isFixedArray(void) const override { return true; }
		bool isVector(void) const override { return false; }
		bool isReadOnly(void) const { return _read_only; }
		int32_t size(const void*) const override { return static_cast<int32_t>(array_size); }

		const void* getElement(const void* object, int32_t index) const override;
		void setElement(void* object, int32_t index, const void* data) override;
		void setElementMove(void* object, int32_t index, void* data) override;
		void swap(void* object, int32_t index_a, int32_t index_b) override;
		void resize(void* object, size_t new_size) override;

		void load(const ISerializeReader& reader, T& object) override;
		void save(ISerializeWriter& writer, const T& object) override;

	private:
		Var (T::*_ptr)[array_size] = nullptr;
		bool _read_only = false;
	};

	template <class Var, class Vec_Allocator>
	class VectorPtr final : public IVar
	{
	public:
		VectorPtr(Vector<Var, Vec_Allocator> T::*ptr, bool read_only);

		ReflectionValueType getType(void) const override;
		const void* getData(const void* object) const override;
		void setData(void* object, const void* data) override;
		void setDataMove(void* object, void* data) override;

		bool isFixedArray(void) const override { return false; }
		bool isVector(void) const override { return true; }
		bool isReadOnly(void) const { return _read_only; }
		int32_t size(const void* object) const override;

		const void* getElement(const void* object, int32_t index) const override;
		void setElement(void* object, int32_t index, const void* data) override;
		void setElementMove(void* object, int32_t index, void* data) override;
		void swap(void* object, int32_t index_a, int32_t index_b) override;
		void resize(void* object, size_t new_size) override;

		void load(const ISerializeReader& reader, T& object) override;
		void save(ISerializeWriter& writer, const T& object) override;

	private:
		Vector<Var, Vec_Allocator> T::*_ptr = nullptr;
		bool _read_only = false;
	};

	class VirtualDestructor
	{
	public:
		virtual ~VirtualDestructor(void) {}
	};

	template <class Ret, class... Args>
	class ReflectionFunction : public IReflectionFunction<Ret, Args...>, public VirtualDestructor
	{
	public:
		using MemFuncConst = Ret (T::*)(Args...) const;
		using MemFunc = Ret (T::*)(Args...);

		ReflectionFunction(MemFuncConst func, bool is_const):
			_is_const(is_const)
		{
			_func.const_func = func;
		}

		ReflectionFunction(MemFunc func, bool is_const):
			_is_const(is_const)
		{
			_func.non_const_func = func;
		}

		Ret call(const void* obj, Args... args) const override
		{
			GAFF_ASSERT(_is_const);
			const T* const object = reinterpret_cast<const T*>(obj);
			return (object->*_func.const_func)(args...);
		}

		Ret call(void* obj, Args... args) const override
		{
			T* const object = reinterpret_cast<T*>(obj);

			if (_is_const) {
				return (object->*_func.const_func)(args...);
			}

			return (object->*_func.non_const_func)(args...);
		}

		bool isConst(void) const override { return _is_const; }

	private:
		union Func
		{
			MemFuncConst const_func;
			MemFunc non_const_func;
		};

		Func _func;
		bool _is_const;
	};

	using IRefFuncPtr = UniquePtr<VirtualDestructor, Allocator>;
	using IVarPtr = UniquePtr<IVar, Allocator>;

	struct FuncData
	{
		FuncData(void) = default;

		FuncData(const FuncData& rhs)
		{
			*this = rhs;
		}

		FuncData& operator=(const FuncData& rhs)
		{
			FuncData& rhs_cast = const_cast<FuncData&>(rhs);

			for (int32_t i = 0; i < NUM_OVERLOADS; ++i) {
				func[i] = std::move(rhs_cast.func[i]);
			}

			memcpy(hash, rhs.hash, sizeof(Hash64) * NUM_OVERLOADS);
			memcpy(offset, rhs.offset, sizeof(int32_t) * NUM_OVERLOADS);
			return *this;
		}

		static constexpr int32_t NUM_OVERLOADS = 8;
		Hash64 hash[NUM_OVERLOADS];
		IRefFuncPtr func[NUM_OVERLOADS];
		int32_t offset[NUM_OVERLOADS];
	};

	VectorMap<HashString64<Allocator>, ptrdiff_t, Allocator> _base_class_offsets;
	VectorMap<HashString32<Allocator>, IVarPtr, Allocator> _vars;
	VectorMap<HashString32<Allocator>, FuncData, Allocator> _funcs;
	VectorMap<Hash64, VoidFunc, Allocator> _ctors;
	VectorMap<Hash64, const IReflectionDefinition*, Allocator> _base_classes;

	VectorMap<Hash32, Vector<IAttributePtr, Allocator>, Allocator> _var_attrs;
	VectorMap<Hash32, Vector<IAttributePtr, Allocator>, Allocator> _func_attrs;
	Vector<IAttributePtr, Allocator> _class_attrs;

	mutable Allocator _allocator;

	int32_t _base_classes_remaining = 0;
	const char* _attr_file = nullptr;

	template <class Base>
	static void RegisterBaseVariables(void);

	template <class First, class... Rest>
	ReflectionDefinition& addAttributes(Vector<IAttributePtr, Allocator>& attrs, const First& first, const Rest&... rest);
	ReflectionDefinition& addAttributes(Vector<IAttributePtr, Allocator>&);

	template <class RefT, class Allocator>
	friend class ReflectionDefinition;
};

NS_END

#include "Gaff_ReflectionDefinition.inl"

#define BASE(type) base<type>(#type)

#ifdef PLATFORM_WINDOWS
	#pragma warning(pop)
#endif
