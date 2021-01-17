/************************************************************************************
Copyright (C) 2021 by Nicholas LaCroix

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

#include "Gaff_FunctionStackHelpers.h"
#include "Gaff_VectorMap.h"
#include "Gaff_Assert.h"
#include "Gaff_Utils.h"
#include "Gaff_Flags.h"
#include "Gaff_Ops.h"

MSVC_DISABLE_WARNING_PUSH(4505)

NS_GAFF

template <class T, class Allocator>
class ReflectionDefinition final : public IReflectionDefinition
{
public:
	// Add to this list as needed.
	using IReflectionDefinition::getVarAttr;

	using IAttributePtr = UniquePtr<IAttribute, Allocator>;
	using LoadFunc = bool (*)(const ISerializeReader&, T&);
	using SaveFunc = void (*)(ISerializeWriter&, const T&);
	using InstanceHashFunc = Hash64 (*)(const T&, Hash64);

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

		virtual bool load(const ISerializeReader& reader, T& object) = 0;
		virtual void save(ISerializeWriter& writer, const T& object) = 0;
	};

	GAFF_STRUCTORS_DEFAULT(ReflectionDefinition);
	GAFF_NO_COPY(ReflectionDefinition);
	GAFF_NO_MOVE(ReflectionDefinition);

	constexpr static bool IsBuiltIn(void) { return false; }

	template <class... Args>
	T* create(Args&&... args) const;

	const char* getFriendlyName(void) const override;

	bool load(const ISerializeReader& reader, void* object, bool refl_load = false) const override;
	void save(ISerializeWriter& writer, const void* object, bool refl_save = false) const override;
	bool load(const ISerializeReader& reader, T& object, bool refl_load = false) const;
	void save(ISerializeWriter& writer, const T& object, bool refl_save = false) const;

	Hash64 getInstanceHash(const void* object, Hash64 init = k_init_hash64) const override;
	Hash64 getInstanceHash(const T& object, Hash64 init = k_init_hash64) const;
	ReflectionDefinition& setInstanceHash(InstanceHashFunc hash_func);

	const void* getInterface(Hash64 class_hash, const void* object) const override;
	void* getInterface(Hash64 class_hash, void* object) const override;
	bool hasInterface(Hash64 class_hash) const override;

	void setAllocator(const Allocator& allocator);
	IAllocator& getAllocator(void) override;

	const IReflection& getReflectionInstance(void) const override;
	int32_t size(void) const override;

	bool isPolymorphic(void) const override;
	bool isBuiltIn(void) const override;

	int32_t getNumVars(void) const override;
	HashStringView32<> getVarName(int32_t index) const override;
	IReflectionVar* getVar(int32_t index) const override;
	IReflectionVar* getVar(Hash32 name) const override;

	int32_t getNumFuncs(void) const override;
	int32_t getNumFuncOverrides(int32_t index) const override;
	HashStringView32<> getFuncName(int32_t index) const override;
	int32_t getFuncIndex(Hash32 name) const override;

	int32_t getNumStaticFuncs(void) const override;
	int32_t getNumStaticFuncOverrides(int32_t index) const override;
	HashStringView32<> getStaticFuncName(int32_t) const override;
	int32_t getStaticFuncIndex(Hash32 name) const override;

	int32_t getNumClassAttrs(void) const override;
	const IAttribute* getClassAttr(Hash64 attr_name) const override;
	const IAttribute* getClassAttr(int32_t index) const override;
	bool hasClassAttr(Hash64 attr_name) const override;
	void addClassAttr(IAttribute& attribute) override;

	int32_t getNumVarAttrs(Hash32 name) const override;
	const IAttribute* getVarAttr(Hash32 name, Hash64 attr_name) const override;
	const IAttribute* getVarAttr(Hash32 name, int32_t index) const override;
	bool hasVarAttr(Hash64 attr_name) const override;

	int32_t getNumFuncAttrs(Hash64 name_arg_hash) const override;
	const IAttribute* getFuncAttr(Hash64 name_arg_hash, Hash64 attr_name) const override;
	const IAttribute* getFuncAttr(Hash64 name_arg_hash, int32_t index) const override;
	bool hasFuncAttr(Hash64 attr_name) const override;

	int32_t getNumStaticFuncAttrs(Hash64 name_arg_hash) const override;
	const IAttribute* getStaticFuncAttr(Hash64 name_arg_hash, Hash64 attr_name) const override;
	const IAttribute* getStaticFuncAttr(Hash64 name_arg_hash, int32_t index) const override;
	bool hasStaticFuncAttr(Hash64 attr_name) const override;

	int32_t getNumConstructors(void) const override;
	IReflectionStaticFunctionBase* getConstructor(int32_t index) const override;
	VoidFunc getConstructor(Hash64 ctor_hash) const override;

	VoidFunc getFactory(Hash64 ctor_hash) const override;

	IReflectionStaticFunctionBase* getStaticFunc(int32_t name_index, int32_t override_index) const override;
	IReflectionStaticFunctionBase* getStaticFunc(Hash32 name, Hash64 args) const override;
	IReflectionFunctionBase* getFunc(int32_t name_index, int32_t override_index) const override;
	IReflectionFunctionBase* getFunc(Hash32 name, Hash64 args) const override;

	void destroyInstance(void* data) const override;

	IVar* getVarT(int32_t index) const;
	IVar* getVarT(Hash32 name) const;

	ReflectionDefinition& friendlyName(const char* name);

	template <class Base>
	ReflectionDefinition& base(const char* name);

	template <class Base>
	ReflectionDefinition& base(void);

	template <class... Args>
	ReflectionDefinition& ctor(Hash64 factory_hash);

	template <class... Args>
	ReflectionDefinition& ctor(void);

	template <class Var, size_t name_size, class... Attrs>
	ReflectionDefinition& var(const char (&name)[name_size], Var T::*ptr, const Attrs&... attributes);

	template <class Enum, size_t name_size, class... Attrs>
	ReflectionDefinition& var(const char(&name)[name_size], Flags<Enum> T::*ptr, const Attrs&... attributes);

	template <class Ret, class Var, size_t name_size, class... Attrs>
	ReflectionDefinition& var(const char (&name)[name_size], Ret (T::*getter)(void) const, void (T::*setter)(Var), const Attrs&... attributes);

	template <class Ret, class Var, size_t name_size, class... Attrs>
	ReflectionDefinition& var(const char (&name)[name_size], Ret (*getter)(const T&), void (*setter)(T&, Var), const Attrs&... attributes);

	template <class Var, class Vec_Allocator, size_t name_size, class... Attrs>
	ReflectionDefinition& var(const char (&name)[name_size], Vector<Var, Vec_Allocator> T::* vec, const Attrs&... attributes);

	template <class Var, size_t array_size, size_t name_size, class... Attrs>
	ReflectionDefinition& var(const char (&name)[name_size], Var (T::*arr)[array_size], const Attrs&... attributes);

	template <class Key, class Value, class VecMap_Allocator, size_t name_size, class... Attrs>
	ReflectionDefinition& var(const char (&name)[name_size], VectorMap<Key, Value, VecMap_Allocator> T::* vec_map, const Attrs&... attributes);

	template <size_t name_size, class Ret, class... Args, class... Attrs>
	ReflectionDefinition& func(const char (&name)[name_size], Ret (T::*ptr)(Args...) const, const Attrs&... attributes);

	template <size_t name_size, class Ret, class... Args, class... Attrs>
	ReflectionDefinition& func(const char (&name)[name_size], Ret (T::*ptr)(Args...), const Attrs&... attributes);

	template <size_t name_size, class Ret, class... Args, class... Attrs>
	ReflectionDefinition& staticFunc(const char (&name)[name_size], Ret (*func)(Args...), const Attrs&... attributes);

	template <class Other>
	ReflectionDefinition& opAdd(void);
	template <class Other>
	ReflectionDefinition& opSub(void);
	template <class Other>
	ReflectionDefinition& opMul(void);
	template <class Other>
	ReflectionDefinition& opDiv(void);
	template <class Other>
	ReflectionDefinition& opMod(void);

	template <class Other>
	ReflectionDefinition& opBitAnd(void);
	template <class Other>
	ReflectionDefinition& opBitOr(void);
	template <class Other>
	ReflectionDefinition& opBitXor(void);
	template <class Other>
	ReflectionDefinition& opBitShiftLeft(void);
	template <class Other>
	ReflectionDefinition& opBitShiftRight(void);

	template <class Other>
	ReflectionDefinition& opAnd(void);
	template <class Other>
	ReflectionDefinition& opOr(void);

	template <class Other>
	ReflectionDefinition& opEqual(void);
	template <class Other>
	ReflectionDefinition& opLessThan(void);
	template <class Other>
	ReflectionDefinition& opGreaterThan(void);
	template <class Other>
	ReflectionDefinition& opLessThanOrEqual(void);
	template <class Other>
	ReflectionDefinition& opGreaterThanOrEqual(void);

	template <class... Args>
	ReflectionDefinition& opCall(void);

	template <class Other>
	ReflectionDefinition& opIndex(void);

	ReflectionDefinition& opAdd(void);
	ReflectionDefinition& opSub(void);
	ReflectionDefinition& opMul(void);
	ReflectionDefinition& opDiv(void);
	ReflectionDefinition& opMod(void);

	ReflectionDefinition& opBitAnd(void);
	ReflectionDefinition& opBitOr(void);
	ReflectionDefinition& opBitXor(void);
	ReflectionDefinition& opBitNot(void);
	ReflectionDefinition& opBitShiftLeft(void);
	ReflectionDefinition& opBitShiftRight(void);

	ReflectionDefinition& opAnd(void);
	ReflectionDefinition& opOr(void);

	ReflectionDefinition& opEqual(void);
	ReflectionDefinition& opLessThan(void);
	ReflectionDefinition& opGreaterThan(void);
	ReflectionDefinition& opLessThanOrEqual(void);
	ReflectionDefinition& opGreaterThanOrEqual(void);

	ReflectionDefinition& opMinus(void);
	ReflectionDefinition& opPlus(void);

	template <int32_t (*to_string_func)(const T&, char*, int32_t)>
	ReflectionDefinition& opToString(void);

	// apply() is not called on these functions.
	template <class... Attrs>
	ReflectionDefinition& classAttrs(const Attrs&... attributes);

	ReflectionDefinition& version(uint32_t version);

	ReflectionDefinition& serialize(LoadFunc serialize_load, SaveFunc serialize_save = nullptr);

	void finish(void);

private:
	template <class Var>
	class VarPtr final : public IVar
	{
	public:
		VarPtr(Var T::*ptr);

		const IReflection& getReflection(void) const override;
		const void* getData(const void* object) const override;
		void* getData(void* object) override;
		void setData(void* object, const void* data) override;
		void setDataMove(void* object, void* data) override;

		bool load(const ISerializeReader& reader, T& object) override;
		void save(ISerializeWriter& writer, const T& object) override;

		bool isFlags(void) const override;

		void setFlagValue(void* object, int32_t flag_index, bool value) override;
		bool getFlagValue(void* object, int32_t flag_index) const override;

	private:
		Var T::* _ptr = nullptr;
	};

	template <class Enum>
	class VarFlagPtr final : public IVar
	{
	public:
		VarFlagPtr(Flags<Enum> T::* ptr, uint8_t flag_index);

		const IReflection& getReflection(void) const override;
		const void* getData(const void* object) const override;
		void* getData(void* object) override;
		void setData(void* object, const void* data) override;
		void setDataMove(void* object, void* data) override;

		bool load(const ISerializeReader& reader, T& object) override;
		void save(ISerializeWriter& writer, const T& object) override;

	private:
		Flags<Enum> T::* _ptr = nullptr;
		uint8_t _flag_index = static_cast<uint8_t>(-1); // Unless flags have the craziest amount of flags, this should hold all possible flag values.
		bool _cache = false;
	};

	template <class Ret, class Var>
	class VarFuncPtrWithCache final : public IVar
	{
	public:
		using GetterMemberFunc = Ret (T::*)(void) const;
		using SetterMemberFunc = void (T::*)(Var);
		using GetterFunc = Ret (*)(const T&);
		using SetterFunc = void (*)(T&, Var);

		VarFuncPtrWithCache(GetterMemberFunc getter, SetterMemberFunc setter);
		VarFuncPtrWithCache(GetterFunc getter, SetterFunc setter);

		const IReflection& getReflection(void) const override;
		const void* getData(const void* object) const override;
		void* getData(void* object) override;
		void setData(void* object, const void* data) override;
		void setDataMove(void* object, void* data) override;

		bool load(const ISerializeReader& reader, T& object) override;
		void save(ISerializeWriter& writer, const T& object) override;

	private:
		using RetType = typename std::remove_const< typename std::remove_pointer< typename std::remove_reference<Ret>::type >::type >::type;
		using VarType = typename std::remove_const< typename std::remove_pointer< typename std::remove_reference<Var>::type >::type >::type;

		union
		{
			GetterMemberFunc _getter_member = nullptr;
			GetterFunc _getter;
		};

		union
		{
			SetterMemberFunc _setter_member = nullptr;
			SetterFunc _setter;
		};

		bool _member_func = true;

		void callSetter(T& object, Var value) const;
		Ret callGetter(const T& object) const;

		RetType _cache;
	};

	template <class Ret, class Var>
	class VarFuncPtr final : public IVar
	{
	public:
		using GetterMemberFunc = Ret (T::*)(void) const;
		using SetterMemberFunc = void (T::*)(Var);
		using GetterFunc = Ret (*)(const T&);
		using SetterFunc = void (*)(T&, Var);

		VarFuncPtr(GetterMemberFunc getter, SetterMemberFunc setter);
		VarFuncPtr(GetterFunc getter, SetterFunc setter);

		const IReflection& getReflection(void) const override;
		const void* getData(const void* object) const override;
		void* getData(void* object) override;
		void setData(void* object, const void* data) override;
		void setDataMove(void* object, void* data) override;

		bool load(const ISerializeReader& reader, T& object) override;
		void save(ISerializeWriter& writer, const T& object) override;

	private:
		using RetType = typename std::remove_const< typename std::remove_pointer< typename std::remove_reference<Ret>::type >::type >::type;

		union
		{
			GetterMemberFunc _getter_member = nullptr;
			GetterFunc _getter;
		};
		
		union
		{
			SetterMemberFunc _setter_member = nullptr;
			SetterFunc _setter;
		};

		bool _member_func = true;

		void callSetter(T& object, Var value) const;
		Ret callGetter(const T& object) const;
	};

	template <class Base>
	class BaseVarPtr final : public IVar
	{
	public:
		BaseVarPtr(typename ReflectionDefinition<Base, Allocator>::IVar* base_var);

		const IReflection& getReflection(void) const override;
		const void* getData(const void* object) const override;
		void* getData(void* object) override;
		void setData(void* object, const void* data) override;
		void setDataMove(void* object, void* data) override;

		bool isFixedArray(void) const override;
		bool isVector(void) const override;
		bool isFlags(void) const override;
		int32_t size(const void*) const override;

		const void* getElement(const void* object, int32_t index) const override;
		void* getElement(void* object, int32_t index) override;
		void setElement(void* object, int32_t index, const void* data) override;
		void setElementMove(void* object, int32_t index, void* data) override;
		void swap(void* object, int32_t index_a, int32_t index_b) override;
		void resize(void* object, size_t new_size) override;

		bool load(const ISerializeReader& reader, T& object) override;
		void save(ISerializeWriter& writer, const T& object) override;

	private:
		typename ReflectionDefinition<Base, Allocator>::IVar* _base_var;
	};

	template <class Var, size_t array_size>
	class ArrayPtr final : public IVar
	{
	public:
		ArrayPtr(Var (T::*ptr)[array_size]);

		const IReflection& getReflection(void) const override;
		const void* getData(const void* object) const override;
		void* getData(void* object) override;
		void setData(void* object, const void* data) override;
		void setDataMove(void* object, void* data) override;

		bool isFixedArray(void) const override { return true; }
		int32_t size(const void*) const override { return static_cast<int32_t>(array_size); }

		const void* getElement(const void* object, int32_t index) const override;
		void* getElement(void* object, int32_t index) override;
		void setElement(void* object, int32_t index, const void* data) override;
		void setElementMove(void* object, int32_t index, void* data) override;
		void swap(void* object, int32_t index_a, int32_t index_b) override;
		void resize(void* object, size_t new_size) override;

		bool load(const ISerializeReader& reader, T& object) override;
		void save(ISerializeWriter& writer, const T& object) override;

	private:
		Var (T::*_ptr)[array_size] = nullptr;
	};

	template <class Var, class Vec_Allocator>
	class VectorPtr final : public IVar
	{
	public:
		VectorPtr(Vector<Var, Vec_Allocator> T::*ptr);

		const IReflection& getReflection(void) const override;
		const void* getData(const void* object) const override;
		void* getData(void* object) override;
		void setData(void* object, const void* data) override;
		void setDataMove(void* object, void* data) override;

		bool isVector(void) const override { return true; }
		int32_t size(const void* object) const override;

		const void* getElement(const void* object, int32_t index) const override;
		void* getElement(void* object, int32_t index) override;
		void setElement(void* object, int32_t index, const void* data) override;
		void setElementMove(void* object, int32_t index, void* data) override;
		void swap(void* object, int32_t index_a, int32_t index_b) override;
		void resize(void* object, size_t new_size) override;

		bool load(const ISerializeReader& reader, T& object) override;
		void save(ISerializeWriter& writer, const T& object) override;

	private:
		Vector<Var, Vec_Allocator> T::* _ptr = nullptr;
	};

	template <class Key, class Value, class VecMap_Allocator>
	class VectorMapPtr final : public IVar
	{
	public:
		VectorMapPtr(VectorMap<Key, Value, VecMap_Allocator> T::* ptr);

		const IReflection& getReflection(void) const override;
		const IReflection& getReflectionKey(void) const override;
		const void* getData(const void* object) const override;
		void* getData(void* object) override;
		void setData(void* object, const void* data) override;
		void setDataMove(void* object, void* data) override;

		bool isMap(void) const override { return true; }
		int32_t size(const void* object) const override;

		const void* getElement(const void* object, int32_t index) const override;
		void* getElement(void* object, int32_t index) override;
		void setElement(void* object, int32_t index, const void* data) override;
		void setElementMove(void* object, int32_t index, void* data) override;
		void swap(void* object, int32_t index_a, int32_t index_b) override;
		void resize(void* object, size_t new_size) override;

		bool load(const ISerializeReader& reader, T& object) override;
		void save(ISerializeWriter& writer, const T& object) override;

	private:
		VectorMap<Key, Value, VecMap_Allocator> T::* _ptr = nullptr;
	};

	using IRefStaticFuncPtr = UniquePtr<IReflectionStaticFunctionBase, Allocator>;
	using IRefFuncPtr = UniquePtr<IReflectionFunctionBase, Allocator>;

	template <bool is_const, class Ret, class... Args>
	class ReflectionFunction final : public IReflectionFunction<Ret, Args...>
	{
	public:
		template <bool is_const>
		struct MemFuncTypeHelper;

		template <>
		struct MemFuncTypeHelper<true> final
		{
			using Type = Ret (T::*)(Args...) const;
		};

		template <>
		struct MemFuncTypeHelper<false> final
		{
			using Type = Ret (T::*)(Args...);
		};

		using MemFuncType = typename MemFuncTypeHelper<is_const>::Type;

		ReflectionFunction(typename MemFuncType func)
		{
			_func = func;
		}

		bool call(const void* object, const FunctionStackEntry* args, int32_t num_args, FunctionStackEntry& ret, IFunctionStackAllocator& allocator) const override
		{
			GAFF_ASSERT_MSG(is_const, "Reflected function is non-const.");

			if (num_args != static_cast<int32_t>(sizeof...(Args))) {
				// $TODO: Log error.
				return false;
			}

			return call(const_cast<void*>(object), args, num_args, ret, allocator);
		}

		bool call(void* object, const FunctionStackEntry* args, int32_t num_args, FunctionStackEntry& ret, IFunctionStackAllocator& allocator) const override
		{
			if (num_args != static_cast<int32_t>(sizeof...(Args))) {
				// $TODO: Log error.
				return false;
			}

			if constexpr (sizeof...(Args) > 0) {
				return CallFunc<ReflectionFunction<is_const, Ret, Args...>, Allocator, Ret, Args...>(*this, object, args, ret, 0, allocator);
			} else {
				GAFF_REF(args);
				return CallFunc<ReflectionFunction<is_const, Ret, Args...>, Allocator, Ret>(*this, object, ret, allocator);
			}
		}

		Ret call(const void* object, Args&&... args) const override
		{
			GAFF_ASSERT_MSG(is_const, "Reflected function is non-const.");
			return call(const_cast<void*>(object), std::forward<Args>(args)...);
		}

		Ret call(void* object, Args&&... args) const override
		{
			return (reinterpret_cast<T*>(object)->*_func)(std::forward<Args>(args)...);
		}

		bool isConst(void) const override { return is_const; }
		const IReflectionDefinition& getBaseRefDef(void) const override { return GAFF_REFLECTION_NAMESPACE::Reflection<T>::GetReflectionDefinition(); }

	private:
		typename MemFuncType _func;
	};

	class ReflectionBaseFunction final : public IReflectionFunctionBase
	{
	public:
		ReflectionBaseFunction(const IReflectionDefinition& base_ref_def, const IReflectionFunctionBase* ref_func):
			_base_ref_def(base_ref_def), _func(ref_func)
		{
		}

		bool call(const void* object, const FunctionStackEntry* args, int32_t num_args, FunctionStackEntry& ret, IFunctionStackAllocator& allocator) const override
		{
			return _func->call(object, args, num_args, ret, allocator);
		}

		bool call(void* object, const FunctionStackEntry* args, int32_t num_args, FunctionStackEntry& ret, IFunctionStackAllocator& allocator) const override
		{
			return _func->call(object, args, num_args, ret, allocator);
		}

		int32_t numArgs(void) const override { return _func->numArgs(); }
		bool isConst(void) const override { return _func->isConst(); }
		bool isBase(void) const override { return true; }
		const IReflectionDefinition& getBaseRefDef(void) const override { return _base_ref_def; }

		template <class Ret, class... Args>
		Ret call(const void* obj, Args&&... args) const
		{
			GAFF_ASSERT(isConst());

			const auto& ref_def = GAFF_REFLECTION_NAMESPACE::Reflection<T>::GetReflectionDefinition();
			const void* const object = ref_def.getInterface(_func->getBaseRefDef().getReflectionInstance().getHash(), obj);

			if (_func->isBase()) {
				return reinterpret_cast<const ReflectionBaseFunction*>(_func)->call<Ret, Args...>(object, std::forward<Args>(args)...);
			} else {
				return reinterpret_cast<const ReflectionFunction<Ret, Args...>*>(_func)->call(object, std::forward<Args>(args)...);
			}
		}

		template <class Ret, class... Args>
		Ret call(void* obj, Args&&... args) const
		{
			const auto& ref_def = GAFF_REFLECTION_NAMESPACE::Reflection<T>::GetReflectionDefinition();
			void* const object = ref_def.getInterface(_func->getBaseRefDef().getReflectionInstance().getHash(), obj);

			if (_func->isBase()) {
				return reinterpret_cast<const ReflectionBaseFunction*>(_func)->call<Ret, Args...>(object, std::forward<Args>(args)...);
			} else {
				return reinterpret_cast< const ReflectionFunction<Ret, Args...>* >(_func)->call(object, std::forward<Args>(args)...);
			}
		}

	private:
		const IReflectionDefinition& _base_ref_def;
		const IReflectionFunctionBase* const _func;
	};

	template <class Ret, class... Args>
	class StaticFunction final : public IReflectionStaticFunction<Ret, Args...>
	{
	public:
		using Func = Ret (*)(Args...);

		explicit StaticFunction(Func func):
			IReflectionStaticFunction<Ret, Args...>(func)
		{
		}

		int32_t numArgs(void) const override { return static_cast<int32_t>(sizeof...(Args)); }

		IReflectionStaticFunctionBase* clone(IAllocator& allocator) const
		{
			using Type = StaticFunction<Ret, Args...>;
			return SHIB_ALLOCT(Type, allocator, reinterpret_cast<Func>(_func));
		}

		bool call(const FunctionStackEntry* args, int32_t num_args, FunctionStackEntry& ret, IFunctionStackAllocator& allocator) const override
		{
			if (num_args != static_cast<int32_t>(sizeof...(Args))) {
				// $TODO: Log error.
				return false;
			}

			if constexpr (sizeof...(Args) > 0) {
				return CallFunc<StaticFunction<Ret, Args...>, Allocator, Ret, Args...>(*this, nullptr, args, ret, 0, allocator);
			} else {
				GAFF_REF(args);
				return CallFunc<StaticFunction<Ret, Args...>, Allocator, Ret>(*this, nullptr, ret, allocator);
			}
		}
	};


	using IVarPtr = UniquePtr<IVar, Allocator>;

	struct FuncData final
	{
		FuncData(void) = default;

		FuncData(const FuncData& rhs)
		{
			*this = rhs;
		}

		FuncData& operator=(const FuncData& rhs)
		{
			FuncData& rhs_cast = const_cast<FuncData&>(rhs);

			for (int32_t i = 0; i < k_num_overloads; ++i) {
				func[i] = std::move(rhs_cast.func[i]);
			}

			memcpy(hash, rhs.hash, sizeof(hash));
			return *this;
		}

		constexpr static int32_t k_num_overloads = 8;
		Hash64 hash[k_num_overloads] =
		{
			Hash64(0), Hash64(0), Hash64(0), Hash64(0),
			Hash64(0), Hash64(0), Hash64(0), Hash64(0)
		};

		IRefFuncPtr func[k_num_overloads];
	};

	struct StaticFuncData
	{
		StaticFuncData(void) = default;

		StaticFuncData(const Allocator& allocator)
		{
			_allocator = allocator;
		}

		StaticFuncData(const StaticFuncData& rhs)
		{
			*this = rhs;
		}

		StaticFuncData& operator=(const StaticFuncData& rhs)
		{
			StaticFuncData& rhs_cast = const_cast<StaticFuncData&>(rhs);

			for (int32_t i = 0; i < k_num_overloads; ++i) {
				func[i] = std::move(rhs_cast.func[i]);
			}

			memcpy(hash, rhs.hash, sizeof(hash));
			return *this;
		}

		Allocator _allocator;
		constexpr static int32_t k_num_overloads = 8;
		Hash64 hash[k_num_overloads] =
		{
			Hash64(0), Hash64(0), Hash64(0), Hash64(0),
			Hash64(0), Hash64(0), Hash64(0), Hash64(0)
		};

		IRefStaticFuncPtr func[k_num_overloads];
	};

	VectorMap<HashString64<Allocator>, ptrdiff_t, Allocator> _base_class_offsets;
	VectorMap<HashString32<Allocator>, IVarPtr, Allocator> _vars;
	VectorMap<HashString32<Allocator>, FuncData, Allocator> _funcs;
	VectorMap<HashString32<Allocator>, StaticFuncData, Allocator> _static_funcs;
	VectorMap<Hash64, VoidFunc, Allocator> _factories;
	VectorMap<Hash64, IRefStaticFuncPtr, Allocator> _ctors;
	VectorMap<Hash64, const IReflectionDefinition*, Allocator> _base_classes;

	using AttributeList = Vector<IAttributePtr, Allocator>;

	VectorMap<Hash32, AttributeList, Allocator> _var_attrs;
	VectorMap<Hash64, AttributeList, Allocator> _func_attrs;
	VectorMap<Hash64, AttributeList, Allocator> _static_func_attrs;
	AttributeList _class_attrs;

	U8String<Allocator> _friendly_name;

	InstanceHashFunc _instance_hash = nullptr;
	LoadFunc _serialize_load = nullptr;
	SaveFunc _serialize_save = nullptr;

	mutable Allocator _allocator;

	int32_t _dependents_remaining = 0;

	template <class Base>
	static void RegisterBaseVariables(void);

	// Variables
	template <class Var, class First, class... Rest>
	ReflectionDefinition& addAttributes(IReflectionVar* ref_var, Var T::*var, Vector<IAttributePtr, Allocator>& attrs, const First& first, const Rest&... rest);
	template <class Var, class Ret, class First, class... Rest>
	ReflectionDefinition& addAttributes(IReflectionVar* ref_var, Ret (T::*getter)(void) const, void (T::*setter)(Var), Vector<IAttributePtr, Allocator>& attrs, const First& first, const Rest&... rest);

	// Functions
	template <class Ret, class... Args, class First, class... Rest>
	ReflectionDefinition& addAttributes(Ret (T::*func)(Args...) const, Vector<IAttributePtr, Allocator>& attrs, const First& first, const Rest&... rest);
	template <class Ret, class... Args, class First, class... Rest>
	ReflectionDefinition& addAttributes(Ret (T::*func)(Args...), Vector<IAttributePtr, Allocator>& attrs, const First& first, const Rest&... rest);

	// Static Functions
	template <class Ret, class... Args, class First, class... Rest>
	ReflectionDefinition& addAttributes(Ret (*func)(Args...), Vector<IAttributePtr, Allocator>& attrs, const First& first, const Rest&... rest);

	// Non-apply() call version.
	template <class First, class... Rest>
	ReflectionDefinition& addAttributes(Vector<IAttributePtr, Allocator>& attrs, const First& first, const Rest&... rest);

	ptrdiff_t getBasePointerOffset(Hash64 interface_name) const override;
	void instantiated(void* object) const override;

	const IAttribute* getAttribute(const AttributeList& attributes, Hash64 attr_name) const;

	template <class RefT, class AllocatorT>
	friend class ReflectionDefinition;
};


template <class T, class... Args>
void ConstructFunc(void* obj, Args&&... args);

template <class T, class... Args>
void* FactoryFunc(IAllocator& allocator, Args&&... args);

#define REF_DEF_BUILTIN(class_type, serialize_type) \
	template <class Allocator> \
	class ReflectionDefinition<class_type, Allocator> final : public IReflectionDefinition \
	{ \
	public: \
		constexpr static bool IsBuiltIn(void) { return true; } \
		const IReflection& getReflectionInstance(void) const override { return GAFF_REFLECTION_NAMESPACE::Reflection<class_type>::GetInstance(); } \
		int32_t size(void) const override { return sizeof(class_type); } \
		bool isPolymorphic(void) const override { return std::is_polymorphic<class_type>::value; } \
		bool isBuiltIn(void) const { return true; } \
		const char* getFriendlyName(void) const override { return GAFF_STR(class_type); } \
		bool load(const ISerializeReader& reader, void* object, bool refl_load = false) const override { return load(reader, *reinterpret_cast<class_type*>(object), refl_load); } \
		void save(ISerializeWriter& writer, const void* object, bool refl_save = false) const override { save(writer, *reinterpret_cast<const class_type*>(object), refl_save); } \
		bool load(const ISerializeReader& reader, class_type& out, bool refl_load = false) const \
		{ \
			GAFF_REF(refl_load); \
			if (!reader.is##serialize_type()) { \
				return false; \
			} \
			out = reader.read##serialize_type(); \
			return true; \
		} \
		void save(ISerializeWriter& writer, const class_type& value, bool refl_save = false) const { GAFF_REF(refl_save); writer.write##serialize_type(value); } \
		Hash64 getInstanceHash(const void* object, Hash64 init = k_init_hash64) const override { return FNV1aHash64(reinterpret_cast<const char*>(object), sizeof(class_type), init); } \
		const void* getInterface(Hash64, const void*) const override { return nullptr; } \
		void* getInterface(Hash64, void*) const override { return nullptr; } \
		bool hasInterface(Hash64) const override { return false; } \
		IAllocator& getAllocator(void) override { return _allocator; } \
		int32_t getNumVars(void) const override { return 0; } \
		HashStringView32<> getVarName(int32_t) const override { return HashStringView32<>(); } \
		IReflectionVar* getVar(int32_t) const override { return nullptr; } \
		IReflectionVar* getVar(Hash32) const override { return nullptr; } \
		int32_t getNumFuncs(void) const override { return 0; } \
		int32_t getNumFuncOverrides(int32_t) const override { return 0; } \
		HashStringView32<> getFuncName(int32_t) const override { return HashStringView32<>(); } \
		int32_t getFuncIndex(Hash32) const override { return -1; } \
		int32_t getNumStaticFuncs(void) const override { return 0; } \
		HashStringView32<> getStaticFuncName(int32_t) const override { return HashStringView32<>(); } \
		int32_t getNumStaticFuncOverrides(int32_t) const override { return 0; } \
		int32_t getStaticFuncIndex(Hash32) const override { return -1; } \
		int32_t getNumClassAttrs(void) const override { return 0; } \
		const IAttribute* getClassAttr(Hash64) const override { return nullptr; } \
		const IAttribute* getClassAttr(int32_t) const override { return nullptr; } \
		bool hasClassAttr(Hash64) const override { return false; } \
		void addClassAttr(IAttribute&) override {} \
		int32_t getNumVarAttrs(Hash32) const override { return 0; } \
		const IAttribute* getVarAttr(Hash32, Hash64) const { return nullptr; } \
		const IAttribute* getVarAttr(Hash32, int32_t) const override { return nullptr; } \
		bool hasVarAttr(Hash64) const override { return false; } \
		int32_t getNumFuncAttrs(Hash64) const override { return 0; } \
		const IAttribute* getFuncAttr(Hash64, Hash64) const { return nullptr; } \
		const IAttribute* getFuncAttr(Hash64, int32_t) const override { return nullptr; } \
		bool hasFuncAttr(Hash64) const override { return false; } \
		int32_t getNumStaticFuncAttrs(Hash64) const override { return 0; } \
		const IAttribute* getStaticFuncAttr(Hash64, Hash64) const { return nullptr; } \
		const IAttribute* getStaticFuncAttr(Hash64, int32_t) const override { return nullptr; } \
		bool hasStaticFuncAttr(Hash64) const override { return false; } \
		int32_t getNumConstructors(void) const override { return 0; } \
		IReflectionStaticFunctionBase* getConstructor(int32_t) const { return nullptr; } \
		VoidFunc getConstructor(Hash64 ctor_hash) const override \
		{ \
			if (ctor_hash == CalcTemplateHash<class_type>(k_init_hash64)) { \
				ConstructFuncT<class_type, class_type> construct_func = Gaff::ConstructFunc<class_type, class_type>; \
				return reinterpret_cast<VoidFunc>(construct_func); \
			} else if (ctor_hash == CalcTemplateHash<>(k_init_hash64)) { \
				ConstructFuncT<class_type> construct_func = Gaff::ConstructFunc<class_type>; \
				return reinterpret_cast<VoidFunc>(construct_func); \
			} \
			return nullptr; \
		} \
		VoidFunc getFactory(Hash64 factory_hash) const override \
		{ \
			if (factory_hash == CalcTemplateHash<class_type>(k_init_hash64)) { \
				FactoryFuncT<class_type, class_type> factory_func = Gaff::FactoryFunc<class_type, class_type>; \
				return reinterpret_cast<VoidFunc>(factory_func); \
			} else if (factory_hash == CalcTemplateHash<>(k_init_hash64)) { \
				FactoryFuncT<class_type> factory_func = Gaff::FactoryFunc<class_type>; \
				return reinterpret_cast<VoidFunc>(factory_func); \
			} \
			return nullptr; \
		} \
		IReflectionStaticFunctionBase* getStaticFunc(int32_t, int32_t) const override { return nullptr; } \
		IReflectionStaticFunctionBase* getStaticFunc(Hash32, Hash64) const override { return nullptr; } \
		IReflectionFunctionBase* getFunc(int32_t, int32_t) const override { return nullptr; } \
		IReflectionFunctionBase* getFunc(Hash32, Hash64) const override { return nullptr; } \
		void destroyInstance(void* data) const override { class_type* const instance = reinterpret_cast<class_type*>(data); Gaff::Deconstruct(instance); } \
		void setAllocator(const Allocator&) {} \
		void finish(void) {} \
	private: \
		Allocator _allocator; \
		ptrdiff_t getBasePointerOffset(Hash64) const override { return 0; } \
		void instantiated(void*) const override {} \
	}

REF_DEF_BUILTIN(int8_t, Int8);
REF_DEF_BUILTIN(int16_t, Int16);
REF_DEF_BUILTIN(int32_t, Int32);
REF_DEF_BUILTIN(int64_t, Int64);
REF_DEF_BUILTIN(uint8_t, UInt8);
REF_DEF_BUILTIN(uint16_t, UInt16);
REF_DEF_BUILTIN(uint32_t, UInt32);
REF_DEF_BUILTIN(uint64_t, UInt64);
REF_DEF_BUILTIN(float, Float);
REF_DEF_BUILTIN(double, Double);
REF_DEF_BUILTIN(bool, Bool);


template <class T>
struct IsVectorHelper final
{
	using type = T;
	static constexpr bool value = false;
};

template <class T, class Allocator>
struct IsVectorHelper< Vector<T, Allocator> > final
{
	using type = typename std::remove_const<T>::type;
	static constexpr bool value = true;
};

template <class T>
static constexpr bool IsVector = IsVectorHelper<T>::value;

template <class T>
using IsVectorType = typename IsVectorHelper<T>::type;


template <class T>
struct IsU8StringRef final
{
	static constexpr bool value = false;
};

template <class Allocator>
struct IsU8StringRef<const U8String<Allocator>&> final
{
	static constexpr bool value = true;
};

template <class T>
struct IsU8String final
{
	static constexpr bool value = false;
};

template <class Allocator>
struct IsU8String< U8String<Allocator> > final
{
	static constexpr bool value = true;
};

template <class T>
struct IsHashStringView final
{
	static constexpr bool value = false;
};

template <class T, class HashType, HashFunc<HashType> HashingFunc>
struct IsHashStringView< const HashStringView<T, HashType, HashingFunc>& > final
{
	static constexpr bool value = true;
};

template <class T, class HashType, HashFunc<HashType> HashingFunc>
struct IsHashStringView< HashStringView<T, HashType, HashingFunc> > final
{
	static constexpr bool value = true;
};

NS_END

#include "Gaff_ReflectionDefinition.inl"

#define CLASS_HASH(Class) Gaff::FNV1aHash64Const(#Class)
#define ARG_HASH(...) Gaff::CalcTemplateHash<__VA_ARGS__>(Gaff::k_init_hash64, eastl::array<const char*, Gaff::GetNumArgs<__VA_ARGS__>()>{ GAFF_FOR_EACH_COMMA(GAFF_STR, __VA_ARGS__) })
#define BASE(type) template base<type>(#type)
#define CTOR(...) ctor<__VA_ARGS__>(ARG_HASH(__VA_ARGS__))
#define GET_INTERFACE(Class, data) getInterface<Class>(CLASS_HASH(Class), data)

MSVC_DISABLE_WARNING_POP()
