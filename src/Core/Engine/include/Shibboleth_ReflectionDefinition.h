/************************************************************************************
Copyright (C) 2023 by Nicholas LaCroix

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

#include "Reflection/Shibboleth_ReflectionDefinitionVariable.h"
#include "Shibboleth_IReflectionDefinition.h"
#include "Shibboleth_AppConfigs.h"
#include "Shibboleth_SmartPtrs.h"
#include "Shibboleth_Utilities.h"
#include "Shibboleth_String.h"
#include <Gaff_JSON.h>
#include <Gaff_Ops.h>

NS_REFLECTION

template <bool is_const, class T, class Ret, class... Args>
struct MemFuncTypeHelper;

template <class T, class Ret, class... Args>
struct MemFuncTypeHelper<true, T, Ret, Args...> final
{
	using Type = Ret (T::*)(Args...) const;
};

template <class T, class Ret, class... Args>
struct MemFuncTypeHelper<false, T, Ret, Args...> final
{
	using Type = Ret (T::*)(Args...);
};

template <bool is_const, class T, class Ret, class... Args>
using MemFuncType = typename MemFuncTypeHelper<is_const, T, Ret, Args...>::Type;


template <class T>
class ReflectionDefinition final : public IReflectionDefinition
{
public:
	// Add to this list as needed.
	using IReflectionDefinition::getVarAttr;

	using IAttributePtr = Shibboleth::UniquePtr<IAttribute>;
	using LoadFunc = bool (*)(const Shibboleth::ISerializeReader&, T&);
	using SaveFunc = void (*)(Shibboleth::ISerializeWriter&, const T&);
	using InstanceHashFunc = Gaff::Hash64 (*)(const T&, Gaff::Hash64);

	GAFF_STRUCTORS_DEFAULT(ReflectionDefinition);
	GAFF_NO_COPY(ReflectionDefinition);
	GAFF_NO_MOVE(ReflectionDefinition);

	constexpr static bool IsBuiltIn(void) { return false; }

	template <class... Args>
	T* create(Args&&... args) const;

	const char8_t* getFriendlyName(void) const override;

	bool load(const Shibboleth::ISerializeReader& reader, void* object, bool refl_load = false) const override;
	void save(Shibboleth::ISerializeWriter& writer, const void* object, bool refl_save = false) const override;
	bool load(const Shibboleth::ISerializeReader& reader, T& object, bool refl_load = false) const;
	void save(Shibboleth::ISerializeWriter& writer, const T& object, bool refl_save = false) const;

	Gaff::Hash64 getInstanceHash(const void* object, Gaff::Hash64 init = Gaff::k_init_hash64) const override;
	Gaff::Hash64 getInstanceHash(const T& object, Gaff::Hash64 init = Gaff::k_init_hash64) const;
	ReflectionDefinition& setInstanceHash(InstanceHashFunc hash_func);

	const void* getInterface(Gaff::Hash64 class_hash, const void* object) const override;
	void* getInterface(Gaff::Hash64 class_hash, void* object) const override;
	bool hasInterface(Gaff::Hash64 class_hash) const override;

	void setAllocator(const Shibboleth::ProxyAllocator& allocator);
	Shibboleth::ProxyAllocator& getAllocator(void) override;

	const IReflection& getReflectionInstance(void) const override;
	//void setReflectionInstance(const IReflection& ref_inst);

	int32_t size(void) const override;

	bool isPolymorphic(void) const override;
	bool isBuiltIn(void) const override;

	int32_t getNumVars(void) const override;
	Shibboleth::HashStringView32<> getVarName(int32_t index) const override;
	IReflectionVar* getVar(int32_t index) const override;
	IReflectionVar* getVar(Gaff::Hash32 name) const override;

	int32_t getNumFuncs(void) const override;
	int32_t getNumFuncOverrides(int32_t index) const override;
	Shibboleth::HashStringView32<> getFuncName(int32_t index) const override;
	int32_t getFuncIndex(Gaff::Hash32 name) const override;

	int32_t getNumStaticFuncs(void) const override;
	int32_t getNumStaticFuncOverrides(int32_t index) const override;
	Shibboleth::HashStringView32<> getStaticFuncName(int32_t) const override;
	int32_t getStaticFuncIndex(Gaff::Hash32 name) const override;

	int32_t getNumClassAttrs(void) const override;
	const IAttribute* getClassAttr(Gaff::Hash64 attr_name) const override;
	const IAttribute* getClassAttr(int32_t index) const override;
	bool hasClassAttr(Gaff::Hash64 attr_name) const override;
	void addClassAttr(IAttribute& attribute) override;

	int32_t getNumVarAttrs(Gaff::Hash32 name) const override;
	const IAttribute* getVarAttr(Gaff::Hash32 name, Gaff::Hash64 attr_name) const override;
	const IAttribute* getVarAttr(Gaff::Hash32 name, int32_t index) const override;
	bool hasVarAttr(Gaff::Hash64 attr_name) const override;

	int32_t getNumFuncAttrs(Gaff::Hash64 name_arg_hash) const override;
	const IAttribute* getFuncAttr(Gaff::Hash64 name_arg_hash, Gaff::Hash64 attr_name) const override;
	const IAttribute* getFuncAttr(Gaff::Hash64 name_arg_hash, int32_t index) const override;
	bool hasFuncAttr(Gaff::Hash64 attr_name) const override;

	int32_t getNumStaticFuncAttrs(Gaff::Hash64 name_arg_hash) const override;
	const IAttribute* getStaticFuncAttr(Gaff::Hash64 name_arg_hash, Gaff::Hash64 attr_name) const override;
	const IAttribute* getStaticFuncAttr(Gaff::Hash64 name_arg_hash, int32_t index) const override;
	bool hasStaticFuncAttr(Gaff::Hash64 attr_name) const override;

	int32_t getNumConstructors(void) const override;
	IReflectionStaticFunctionBase* getConstructor(int32_t index) const override;
	VoidFunc getConstructor(Gaff::Hash64 ctor_hash) const override;

	VoidFunc getFactory(Gaff::Hash64 ctor_hash) const override;

	IReflectionStaticFunctionBase* getStaticFunc(int32_t name_index, int32_t override_index) const override;
	IReflectionStaticFunctionBase* getStaticFunc(Gaff::Hash32 name, Gaff::Hash64 args) const override;
	IReflectionFunctionBase* getFunc(int32_t name_index, int32_t override_index) const override;
	IReflectionFunctionBase* getFunc(Gaff::Hash32 name, Gaff::Hash64 args) const override;

	void* duplicate(const void* object, Gaff::IAllocator& allocator) const override;

	void destroyInstance(void* data) const override;

	IVar<T>* getVarT(int32_t index) const;
	IVar<T>* getVarT(Gaff::Hash32 name) const;

	ReflectionDefinition& friendlyName(const char8_t* name);

	template <class Base>
	ReflectionDefinition& base(const char8_t* name);

	template <class Base>
	ReflectionDefinition& base(void);

	template <class... Args>
	ReflectionDefinition& ctor(Gaff::Hash64 factory_hash);

	template <class... Args>
	ReflectionDefinition& ctor(void);

	// This is meant to be used for scenarios like SerializeablePtr, where we don't want a ReflectionDefinition for every template instantiation.
	template <class Base, class Var, size_t name_size, class... Attrs>
	ReflectionDefinition& varUsingBase(const char8_t (&name)[name_size], Var T::* ptr, const Attrs&... attributes);

	template <class Base, class Var, size_t name_size, class... Attrs>
	ReflectionDefinition& varUsingBase(const char (&name)[name_size], Var T::* ptr, const Attrs&... attributes);

	template <class Base, class Var, class Vec_Allocator, size_t name_size, class... Attrs>
	ReflectionDefinition& varUsingBase(const char8_t (&name)[name_size], Gaff::Vector<Var, Vec_Allocator> T::* vec, const Attrs&... attributes);

	template <class Base, class Var, class Vec_Allocator, size_t name_size, class... Attrs>
	ReflectionDefinition& varUsingBase(const char (&name)[name_size], Gaff::Vector<Var, Vec_Allocator> T::* vec, const Attrs&... attributes);

	template <class Base, class Var, size_t array_size, size_t name_size, class... Attrs>
	ReflectionDefinition& varUsingBase(const char8_t (&name)[name_size], Var (T::*arr)[array_size], const Attrs&... attributes);

	template <class Base, class Var, size_t array_size, size_t name_size, class... Attrs>
	ReflectionDefinition& varUsingBase(const char (&name)[name_size], Var (T::*arr)[array_size], const Attrs&... attributes);


	template <class Var, size_t name_size, class... Attrs>
	ReflectionDefinition& var(const char8_t (&name)[name_size], Var T::* ptr, const Attrs&... attributes);

	template <class Var, size_t name_size, class... Attrs>
	ReflectionDefinition& var(const char (&name)[name_size], Var T::* ptr, const Attrs&... attributes);

	template <class Ret, class Var, size_t name_size, class... Attrs>
	ReflectionDefinition& var(const char8_t (&name)[name_size], Ret (T::*getter)(void) const, void (T::*setter)(Var), const Attrs&... attributes);

	template <class Ret, class Var, size_t name_size, class... Attrs>
	ReflectionDefinition& var(const char (&name)[name_size], Ret (T::*getter)(void) const, void (T::*setter)(Var), const Attrs&... attributes);

	template <class Ret, class Var, size_t name_size, class... Attrs>
	ReflectionDefinition& var(const char8_t (&name)[name_size], Ret (*getter)(const T&), void (*setter)(T&, Var), const Attrs&... attributes);

	template <class Ret, class Var, size_t name_size, class... Attrs>
	ReflectionDefinition& var(const char (&name)[name_size], Ret (*getter)(const T&), void (*setter)(T&, Var), const Attrs&... attributes);

	template <class Var, class Vec_Allocator, size_t name_size, class... Attrs>
	ReflectionDefinition& var(const char8_t (&name)[name_size], Gaff::Vector<Var, Vec_Allocator> T::* vec, const Attrs&... attributes);

	template <class Var, class Vec_Allocator, size_t name_size, class... Attrs>
	ReflectionDefinition& var(const char (&name)[name_size], Gaff::Vector<Var, Vec_Allocator> T::* vec, const Attrs&... attributes);

	template <class Var, size_t array_size, size_t name_size, class... Attrs>
	ReflectionDefinition& var(const char8_t (&name)[name_size], Var (T::*arr)[array_size], const Attrs&... attributes);

	template <class Var, size_t array_size, size_t name_size, class... Attrs>
	ReflectionDefinition& var(const char (&name)[name_size], Var (T::*arr)[array_size], const Attrs&... attributes);

	template <class Key, class Value, class VecMap_Allocator, size_t name_size, class... Attrs>
	ReflectionDefinition& var(const char8_t (&name)[name_size], Gaff::VectorMap<Key, Value, VecMap_Allocator> T::* vec_map, const Attrs&... attributes);

	template <class Key, class Value, class VecMap_Allocator, size_t name_size, class... Attrs>
	ReflectionDefinition& var(const char (&name)[name_size], Gaff::VectorMap<Key, Value, VecMap_Allocator> T::* vec_map, const Attrs&... attributes);

	template <size_t name_size, class... Attrs>
	ReflectionDefinition& var(const char8_t (&name)[name_size], Gaff::Hash64 T::* ptr, const Attrs&... attributes);

	template <size_t name_size, class... Attrs>
	ReflectionDefinition& var(const char (&name)[name_size], Gaff::Hash64 T::* ptr, const Attrs&... attributes);

	template <size_t name_size, class... Attrs>
	ReflectionDefinition& var(const char8_t (&name)[name_size], Gaff::Hash32 T::* ptr, const Attrs&... attributes);

	template <size_t name_size, class... Attrs>
	ReflectionDefinition& var(const char (&name)[name_size], Gaff::Hash32 T::* ptr, const Attrs&... attributes);

	template <size_t name_size, class Ret, class... Args, class... Attrs>
	ReflectionDefinition& func(const char8_t (&name)[name_size], Ret (T::*ptr)(Args...) const, const Attrs&... attributes);

	template <size_t name_size, class Ret, class... Args, class... Attrs>
	ReflectionDefinition& func(const char (&name)[name_size], Ret (T::*ptr)(Args...) const, const Attrs&... attributes);

	template <size_t name_size, class Ret, class... Args, class... Attrs>
	ReflectionDefinition& func(const char8_t (&name)[name_size], Ret (T::*ptr)(Args...), const Attrs&... attributes);

	template <size_t name_size, class Ret, class... Args, class... Attrs>
	ReflectionDefinition& func(const char (&name)[name_size], Ret (T::*ptr)(Args...), const Attrs&... attributes);

	template <size_t name_size, class Ret, class... Args, class... Attrs>
	ReflectionDefinition& staticFunc(const char8_t (&name)[name_size], Ret (*func)(Args...), const Attrs&... attributes);

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

	template <int32_t (*to_string_func)(const T&, char8_t*, int32_t)>
	ReflectionDefinition& opToString(void);

	// apply() is not called on these functions.
	template <class... Attrs>
	ReflectionDefinition& classAttrs(const Attrs&... attributes);

	ReflectionDefinition& version(uint32_t version);

	ReflectionDefinition& serialize(LoadFunc serialize_load, SaveFunc serialize_save = nullptr);

	void finish(void);

private:
	template <class Var, class Base>
	class VarUsingBasePtr final : public IVar<T>
	{
	public:
		VarUsingBasePtr(Var T::*ptr);

		const IReflection& getReflection(void) const override;
		const void* getData(const void* object) const override;
		void* getData(void* object) override;
		void setData(void* object, const void* data) override;
		void setDataMove(void* object, void* data) override;

		bool load(const Shibboleth::ISerializeReader& reader, T& object) override;
		void save(Shibboleth::ISerializeWriter& writer, const T& object) override;

	private:
		Var T::* _ptr = nullptr;
	};

	template <class Var>
	class VarPtr final : public IVar<T>
	{
	public:
		VarPtr(Var T::*ptr);

		const IReflection& getReflection(void) const override;
		const void* getData(const void* object) const override;
		void* getData(void* object) override;
		void setData(void* object, const void* data) override;
		void setDataMove(void* object, void* data) override;

		bool load(const Shibboleth::ISerializeReader& reader, T& object) override;
		void save(Shibboleth::ISerializeWriter& writer, const T& object) override;

		bool isFlags(void) const override;

		void setFlagValue(void* object, int32_t flag_index, bool value) override;
		bool getFlagValue(void* object, int32_t flag_index) const override;

	private:
		Var T::* _ptr = nullptr;
	};

	template <class Enum>
	class VarFlagPtr final : public IVar<T>
	{
	public:
		VarFlagPtr(Gaff::Flags<Enum> T::* ptr, uint8_t flag_index);

		const IReflection& getReflection(void) const override;
		const void* getData(const void* object) const override;
		void* getData(void* object) override;
		void setData(void* object, const void* data) override;
		void setDataMove(void* object, void* data) override;

		bool load(const Shibboleth::ISerializeReader& reader, T& object) override;
		void save(Shibboleth::ISerializeWriter& writer, const T& object) override;

	private:
		Gaff::Flags<Enum> T::* _ptr = nullptr;
		uint8_t _flag_index = static_cast<uint8_t>(-1); // Unless flags have the craziest amount of flags, this should hold all possible flag values.
		bool _cache = false;
	};

	template <class Ret, class Var>
	class VarFuncPtrWithCache final : public IVar<T>
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

		bool load(const Shibboleth::ISerializeReader& reader, T& object) override;
		void save(Shibboleth::ISerializeWriter& writer, const T& object) override;

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
	class VarFuncPtr final : public IVar<T>
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

		bool load(const Shibboleth::ISerializeReader& reader, T& object) override;
		void save(Shibboleth::ISerializeWriter& writer, const T& object) override;

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
	class BaseVarPtr final : public IVar<T>
	{
	public:
		BaseVarPtr(IVar<Base>* base_var);

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

		bool load(const Shibboleth::ISerializeReader& reader, T& object) override;
		void save(Shibboleth::ISerializeWriter& writer, const T& object) override;

	private:
		IVar<Base>* _base_var;
	};

	template <class Var, class Base, size_t array_size>
	class ArrayUsingBasePtr final : public IVar<T>
	{
	public:
		ArrayUsingBasePtr(Var (T::*ptr)[array_size]);

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

		bool load(const Shibboleth::ISerializeReader& reader, T& object) override;
		void save(Shibboleth::ISerializeWriter& writer, const T& object) override;

	private:
		Var (T::*_ptr)[array_size] = nullptr;
	};

	template <class Var, class Base, class Vec_Allocator>
	class VectorUsingBasePtr final : public IVar<T>
	{
	public:
		VectorUsingBasePtr(Gaff::Vector<Var, Vec_Allocator> T::*ptr);

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

		bool load(const Shibboleth::ISerializeReader& reader, T& object) override;
		void save(Shibboleth::ISerializeWriter& writer, const T& object) override;

	private:
		Gaff::Vector<Var, Vec_Allocator> T::* _ptr = nullptr;
	};

	template <class Var, size_t array_size>
	class ArrayPtr final : public IVar<T>
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

		bool load(const Shibboleth::ISerializeReader& reader, T& object) override;
		void save(Shibboleth::ISerializeWriter& writer, const T& object) override;

	private:
		Var (T::*_ptr)[array_size] = nullptr;
	};

	template <class Var, class Vec_Allocator>
	class VectorPtr final : public IVar<T>
	{
	public:
		VectorPtr(Gaff::Vector<Var, Vec_Allocator> T::*ptr);

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

		bool load(const Shibboleth::ISerializeReader& reader, T& object) override;
		void save(Shibboleth::ISerializeWriter& writer, const T& object) override;

	private:
		Gaff::Vector<Var, Vec_Allocator> T::* _ptr = nullptr;
	};

	template <class Key, class Value, class VecMap_Allocator>
	class VectorMapPtr final : public IVar<T>
	{
	public:
		VectorMapPtr(Gaff::VectorMap<Key, Value, VecMap_Allocator> T::* ptr);

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

		bool load(const Shibboleth::ISerializeReader& reader, T& object) override;
		void save(Shibboleth::ISerializeWriter& writer, const T& object) override;

	private:
		Gaff::VectorMap<Key, Value, VecMap_Allocator> T::* _ptr = nullptr;
	};

	// These are intended for use only when in editor mode.
	class Hash64Ptr final : public IVar<T>
	{
	public:
		Hash64Ptr(Gaff::Hash64 T::* ptr);

		const IReflection& getReflection(void) const override;
		const void* getData(const void* object) const override;
		void* getData(void* object) override;
		void setData(void* object, const void* data) override;
		void setDataMove(void* object, void* data) override;

		bool load(const Shibboleth::ISerializeReader& reader, T& object) override;
		void save(Shibboleth::ISerializeWriter& writer, const T& object) override;

	private:
		Shibboleth::U8String _string;
		Gaff::Hash64 T::* _ptr = nullptr;
	};

	class Hash32Ptr final : public IVar<T>
	{
	public:
		Hash32Ptr(Gaff::Hash32 T::* ptr);

		const IReflection& getReflection(void) const override;
		const void* getData(const void* object) const override;
		void* getData(void* object) override;
		void setData(void* object, const void* data) override;
		void setDataMove(void* object, void* data) override;

		bool load(const Shibboleth::ISerializeReader& reader, T& object) override;
		void save(Shibboleth::ISerializeWriter& writer, const T& object) override;

	private:
		Shibboleth::U8String _string;
		Gaff::Hash32 T::* _ptr = nullptr;
	};


	using IRefStaticFuncPtr = Shibboleth::UniquePtr<IReflectionStaticFunctionBase>;
	using IRefFuncPtr = Shibboleth::UniquePtr<IReflectionFunctionBase>;

	template <bool is_const, class Ret, class... Args>
	class ReflectionFunction final : public IReflectionFunction<Ret, Args...>
	{
	public:
		ReflectionFunction(MemFuncType<is_const, T, Ret, Args...> func)
		{
			_func = func;
		}

		bool callStack(const void* object, const FunctionStackEntry* args, int32_t num_args, FunctionStackEntry& ret, IFunctionStackAllocator& allocator) const override
		{
			GAFF_ASSERT_MSG(is_const, "Reflected function is non-const.");
			return callStack(const_cast<void*>(object), args, num_args, ret, allocator);
		}

		bool callStack(void* object, const FunctionStackEntry* args, int32_t num_args, FunctionStackEntry& ret, IFunctionStackAllocator& allocator) const override
		{
			if (num_args != static_cast<int32_t>(sizeof...(Args))) {
				// $TODO: Log error.
				return false;
			}

			if constexpr (sizeof...(Args) > 0) {
				return CallFuncStackHelper<ReflectionFunction<is_const, Ret, Args...>, Ret, Args...>(*this, object, args, ret, 0, allocator);
			} else {
				GAFF_REF(args);
				return CallFuncStackHelper<ReflectionFunction<is_const, Ret, Args...>, Ret>(*this, object, ret, allocator);
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
		const IReflectionDefinition& getBaseRefDef(void) const override { return Reflection<T>::GetReflectionDefinition(); }

	private:
		MemFuncType<is_const, T, Ret, Args...> _func;
	};

	class ReflectionBaseFunction final : public IReflectionFunctionBase
	{
	public:
		ReflectionBaseFunction(const IReflectionDefinition& base_ref_def, const IReflectionFunctionBase* ref_func):
			_base_ref_def(base_ref_def), _func(ref_func)
		{
		}

		bool callStack(const void* object, const FunctionStackEntry* args, int32_t num_args, FunctionStackEntry& ret, IFunctionStackAllocator& allocator) const override
		{
			return _func->callStack(object, args, num_args, ret, allocator);
		}

		bool callStack(void* object, const FunctionStackEntry* args, int32_t num_args, FunctionStackEntry& ret, IFunctionStackAllocator& allocator) const override
		{
			return _func->callStack(object, args, num_args, ret, allocator);
		}

		int32_t numArgs(void) const override { return _func->numArgs(); }
		bool isConst(void) const override { return _func->isConst(); }
		bool isBase(void) const override { return true; }
		const IReflectionDefinition& getBaseRefDef(void) const override { return _base_ref_def; }

		template <class Ret, class... Args>
		Ret call(const void* obj, Args&&... args) const
		{
			GAFF_ASSERT(isConst());

			const auto& ref_def = Reflection<T>::GetReflectionDefinition();
			const void* const object = ref_def.getInterface(_func->getBaseRefDef().getReflectionInstance().getNameHash(), obj);

			if (_func->isBase()) {
				return reinterpret_cast<const ReflectionBaseFunction*>(_func)->call<Ret, Args...>(object, std::forward<Args>(args)...);
			} else {
				return reinterpret_cast<const ReflectionFunction<true, Ret, Args...>*>(_func)->call(object, std::forward<Args>(args)...);
			}
		}

		template <class Ret, class... Args>
		Ret call(void* obj, Args&&... args) const
		{
			if (isConst()) {
				return const_cast<const ReflectionBaseFunction*>(this)->call(obj, std::forward<Args>(args)...);
			}

			const auto& ref_def = Reflection<T>::GetReflectionDefinition();
			void* const object = ref_def.getInterface(_func->getBaseRefDef().getReflectionInstance().getNameHash(), obj);

			if (_func->isBase()) {
				return reinterpret_cast<const ReflectionBaseFunction*>(_func)->call<Ret, Args...>(object, std::forward<Args>(args)...);
			} else {
				return reinterpret_cast< const ReflectionFunction<false, Ret, Args...>* >(_func)->call(object, std::forward<Args>(args)...);
			}
		}

	private:
		const IReflectionDefinition& _base_ref_def;
		const IReflectionFunctionBase* const _func;
	};

	template <class Ret, class... Args>
	class ReflectionStaticFunction final : public IReflectionStaticFunction<Ret, Args...>
	{
	public:
		using IReflectionStaticFunction<Ret, Args...>::getFunc;
		using IReflectionStaticFunction<Ret, Args...>::numArgs;
		using IReflectionStaticFunction<Ret, Args...>::call;

		explicit ReflectionStaticFunction(typename IReflectionStaticFunction<Ret, Args...>::Func func):
			IReflectionStaticFunction<Ret, Args...>(func)
		{
		}

		IReflectionStaticFunctionBase* clone(Shibboleth::ProxyAllocator& allocator) const override
		{
			using Type = ReflectionStaticFunction<Ret, Args...>;
			return SHIB_ALLOCT(Type, allocator, getFunc());
		}

		bool callStack(const FunctionStackEntry* args, int32_t num_args, FunctionStackEntry& ret, IFunctionStackAllocator& allocator) const override
		{
			if (num_args != static_cast<int32_t>(sizeof...(Args))) {
				// $TODO: Log error.
				return false;
			}

			if constexpr (sizeof...(Args) > 0) {
				return CallFuncStackHelper<ReflectionStaticFunction<Ret, Args...>, Ret, Args...>(*this, nullptr, args, ret, 0, allocator);
			} else {
				GAFF_REF(args);
				return CallFuncStackHelper<ReflectionStaticFunction<Ret, Args...>, Ret>(*this, nullptr, ret, allocator);
			}
		}
	};


	using IVarPtr = Shibboleth::UniquePtr< IVar<T> >;

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
		Gaff::Hash64 hash[k_num_overloads] =
		{
			Gaff::Hash64(0), Gaff::Hash64(0), Gaff::Hash64(0), Gaff::Hash64(0),
			Gaff::Hash64(0), Gaff::Hash64(0), Gaff::Hash64(0), Gaff::Hash64(0)
		};

		IRefFuncPtr func[k_num_overloads];
	};

	struct StaticFuncData
	{
		StaticFuncData(void) = default;

		StaticFuncData(const Shibboleth::ProxyAllocator& allocator)
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

		Shibboleth::ProxyAllocator _allocator;
		constexpr static int32_t k_num_overloads = 8;
		Gaff::Hash64 hash[k_num_overloads] =
		{
			Gaff::Hash64(0), Gaff::Hash64(0), Gaff::Hash64(0), Gaff::Hash64(0),
			Gaff::Hash64(0), Gaff::Hash64(0), Gaff::Hash64(0), Gaff::Hash64(0)
		};

		IRefStaticFuncPtr func[k_num_overloads];
	};

	Shibboleth::VectorMap<Shibboleth::HashString64<>, ptrdiff_t> _base_class_offsets{ Shibboleth::ProxyAllocator("Reflection") };
	Shibboleth::VectorMap<Shibboleth::HashString32<>, IVarPtr> _vars{ Shibboleth::ProxyAllocator("Reflection") };
	Shibboleth::VectorMap<Shibboleth::HashString32<>, FuncData> _funcs{ Shibboleth::ProxyAllocator("Reflection") };
	Shibboleth::VectorMap<Shibboleth::HashString32<>, StaticFuncData> _static_funcs{ Shibboleth::ProxyAllocator("Reflection") };
	Shibboleth::VectorMap<Gaff::Hash64, VoidFunc> _factories{ Shibboleth::ProxyAllocator("Reflection") };
	Shibboleth::VectorMap<Gaff::Hash64, IRefStaticFuncPtr> _ctors{ Shibboleth::ProxyAllocator("Reflection") };
	Shibboleth::VectorMap<Gaff::Hash64, const IReflectionDefinition*> _base_classes{ Shibboleth::ProxyAllocator("Reflection") };

	using AttributeList = Shibboleth::Vector<IAttributePtr>;

	Shibboleth::VectorMap<Gaff::Hash32, AttributeList> _var_attrs{ Shibboleth::ProxyAllocator("Reflection") };
	Shibboleth::VectorMap<Gaff::Hash64, AttributeList> _func_attrs{ Shibboleth::ProxyAllocator("Reflection") };
	Shibboleth::VectorMap<Gaff::Hash64, AttributeList> _static_func_attrs{ Shibboleth::ProxyAllocator("Reflection") };
	AttributeList _class_attrs{ Shibboleth::ProxyAllocator("Reflection") };

	Shibboleth::U8String _friendly_name;

	InstanceHashFunc _instance_hash = nullptr;
	LoadFunc _serialize_load = nullptr;
	SaveFunc _serialize_save = nullptr;

	mutable Shibboleth::ProxyAllocator _allocator;

	//const IReflection* _ref_inst = nullptr;
	int32_t _dependents_remaining = 0;

	template <class Base>
	static void RegisterBaseVariables(void);

	// Variables
	template <class Var, class First, class... Rest>
	ReflectionDefinition& addAttributes(IReflectionVar& ref_var, Var T::*var, Shibboleth::Vector<IAttributePtr>& attrs, const First& first, const Rest&... rest);
	template <class Var, class Ret, class First, class... Rest>
	ReflectionDefinition& addAttributes(IReflectionVar& ref_var, Ret (T::*getter)(void) const, void (T::*setter)(Var), Shibboleth::Vector<IAttributePtr>& attrs, const First& first, const Rest&... rest);

	// Functions
	template <class Ret, class... Args, class First, class... Rest>
	ReflectionDefinition& addAttributes(IReflectionFunction<Ret, Args...>& ref_func, Ret (T::*func)(Args...) const, Shibboleth::Vector<IAttributePtr>& attrs, const First& first, const Rest&... rest);
	template <class Ret, class... Args, class First, class... Rest>
	ReflectionDefinition& addAttributes(IReflectionFunction<Ret, Args...>& ref_func, Ret (T::*func)(Args...), Shibboleth::Vector<IAttributePtr>& attrs, const First& first, const Rest&... rest);

	// Static Functions
	template <class Ret, class... Args, class First, class... Rest>
	ReflectionDefinition& addAttributes(IReflectionStaticFunction<Ret, Args...>& ref_func, Ret (*func)(Args...), Shibboleth::Vector<IAttributePtr>& attrs, const First& first, const Rest&... rest);

	// Non-apply() call version.
	template <class First, class... Rest>
	ReflectionDefinition& addAttributes(Shibboleth::Vector<IAttributePtr>& attrs, const First& first, const Rest&... rest);

	ptrdiff_t getBasePointerOffset(Gaff::Hash64 interface_name) const override;
	void instantiated(void* object) const override;

	const IAttribute* getAttribute(const AttributeList& attributes, Gaff::Hash64 attr_name) const;


	template <class Callable, class Ret, class First, class... Rest, class... CurrentArgs>
	static bool CallFuncStackHelper(
		const Callable& callable,
		void* object,
		const FunctionStackEntry* args,
		FunctionStackEntry& ret,
		int32_t arg_index,
		IFunctionStackAllocator& allocator,
		CurrentArgs&&... current_args
	);

	template <class Callable, class Ret, class... CurrentArgs>
	static bool CallFuncStackHelper(
		const Callable& callable,
		void* object,
		FunctionStackEntry& ret,
		IFunctionStackAllocator& allocator,
		CurrentArgs&&... current_args
	);

	template <bool is_const, class Ret, class... Args, class... CurrentArgs>
	static Ret CallCallableStackHelper(const ReflectionFunction<is_const, Ret, Args...>& func, void* object, CurrentArgs&&... current_args);

	template <class Ret, class... Args, class... CurrentArgs>
	static Ret CallCallableStackHelper(const ReflectionStaticFunction<Ret, Args...>& func, void*, CurrentArgs&&... current_args);


	template <class RefT>
	friend class ReflectionDefinition;

	template <bool is_const, class Ret, class... Args>
	friend class ReflectionFunction;

	template <class Ret, class... Args>
	class ReflectionStaticFunction;
};


template <class T, class... Args>
void ConstructFuncImpl(T* obj, Args&&... args);

template <class T, class... Args>
T* FactoryFuncImpl(Gaff::IAllocator& allocator, Args&&... args);

#define REF_DEF_BUILTIN(class_type, serialize_type) \
	template <> \
	class ReflectionDefinition<class_type> final : public IReflectionDefinition \
	{ \
	public: \
		constexpr static bool IsBuiltIn(void) { return true; } \
		const IReflection& getReflectionInstance(void) const override { return Reflection<class_type>::GetInstance();; } \
		int32_t size(void) const override { return sizeof(class_type); } \
		bool isPolymorphic(void) const override { return std::is_polymorphic<class_type>::value; } \
		bool isBuiltIn(void) const override { return true; } \
		const char8_t* getFriendlyName(void) const override { return GAFF_STR_U8(class_type); } \
		bool load(const Shibboleth::ISerializeReader& reader, void* object, bool refl_load = false) const override { return load(reader, *reinterpret_cast<class_type*>(object), refl_load); } \
		void save(Shibboleth::ISerializeWriter& writer, const void* object, bool refl_save = false) const override { save(writer, *reinterpret_cast<const class_type*>(object), refl_save); } \
		bool load(const Shibboleth::ISerializeReader& reader, class_type& out, bool refl_load = false) const \
		{ \
			GAFF_REF(refl_load); \
			if (!reader.is##serialize_type()) { \
				return false; \
			} \
			out = reader.read##serialize_type(); \
			return true; \
		} \
		void save(Shibboleth::ISerializeWriter& writer, const class_type& value, bool refl_save = false) const { GAFF_REF(refl_save); writer.write##serialize_type(value); } \
		Gaff::Hash64 getInstanceHash(const void* object, Gaff::Hash64 init = Gaff::k_init_hash64) const override { return Gaff::FNV1aHash64(reinterpret_cast<const char*>(object), sizeof(class_type), init); } \
		const void* getInterface(Gaff::Hash64, const void*) const override { return nullptr; } \
		void* getInterface(Gaff::Hash64, void*) const override { return nullptr; } \
		bool hasInterface(Gaff::Hash64) const override { return false; } \
		Shibboleth::ProxyAllocator& getAllocator(void) override { return _allocator; } \
		int32_t getNumVars(void) const override { return 0; } \
		Shibboleth::HashStringView32<> getVarName(int32_t) const override { return Shibboleth::HashStringView32<>(); } \
		IReflectionVar* getVar(int32_t) const override { return nullptr; } \
		IReflectionVar* getVar(Gaff::Hash32) const override { return nullptr; } \
		int32_t getNumFuncs(void) const override { return 0; } \
		int32_t getNumFuncOverrides(int32_t) const override { return 0; } \
		Shibboleth::HashStringView32<> getFuncName(int32_t) const override { return Shibboleth::HashStringView32<>(); } \
		int32_t getFuncIndex(Gaff::Hash32) const override { return -1; } \
		int32_t getNumStaticFuncs(void) const override { return 0; } \
		Shibboleth::HashStringView32<> getStaticFuncName(int32_t) const override { return Shibboleth::HashStringView32<>(); } \
		int32_t getNumStaticFuncOverrides(int32_t) const override { return 0; } \
		int32_t getStaticFuncIndex(Gaff::Hash32) const override { return -1; } \
		int32_t getNumClassAttrs(void) const override { return 0; } \
		const IAttribute* getClassAttr(Gaff::Hash64) const override { return nullptr; } \
		const IAttribute* getClassAttr(int32_t) const override { return nullptr; } \
		bool hasClassAttr(Gaff::Hash64) const override { return false; } \
		void addClassAttr(IAttribute&) override {} \
		int32_t getNumVarAttrs(Gaff::Hash32) const override { return 0; } \
		const IAttribute* getVarAttr(Gaff::Hash32, Gaff::Hash64) const override { return nullptr; } \
		const IAttribute* getVarAttr(Gaff::Hash32, int32_t) const override { return nullptr; } \
		bool hasVarAttr(Gaff::Hash64) const override { return false; } \
		int32_t getNumFuncAttrs(Gaff::Hash64) const override { return 0; } \
		const IAttribute* getFuncAttr(Gaff::Hash64, Gaff::Hash64) const override { return nullptr; } \
		const IAttribute* getFuncAttr(Gaff::Hash64, int32_t) const override { return nullptr; } \
		bool hasFuncAttr(Gaff::Hash64) const override { return false; } \
		int32_t getNumStaticFuncAttrs(Gaff::Hash64) const override { return 0; } \
		const IAttribute* getStaticFuncAttr(Gaff::Hash64, Gaff::Hash64) const override { return nullptr; } \
		const IAttribute* getStaticFuncAttr(Gaff::Hash64, int32_t) const override { return nullptr; } \
		bool hasStaticFuncAttr(Gaff::Hash64) const override { return false; } \
		int32_t getNumConstructors(void) const override { return 0; } \
		IReflectionStaticFunctionBase* getConstructor(int32_t) const override { return nullptr; } \
		VoidFunc getConstructor(Gaff::Hash64 ctor_hash) const override \
		{ \
			if (ctor_hash == Gaff::CalcTemplateHash<class_type>(Gaff::k_init_hash64)) { \
				ConstructFuncT<class_type, class_type> construct_func = ConstructFuncImpl<class_type, class_type>; \
				return reinterpret_cast<VoidFunc>(construct_func); \
			} else if (ctor_hash == Gaff::CalcTemplateHash<>(Gaff::k_init_hash64)) { \
				ConstructFuncT<class_type> construct_func = ConstructFuncImpl<class_type>; \
				return reinterpret_cast<VoidFunc>(construct_func); \
			} \
			return nullptr; \
		} \
		VoidFunc getFactory(Gaff::Hash64 factory_hash) const override \
		{ \
			if (factory_hash == Gaff::CalcTemplateHash<class_type>(Gaff::k_init_hash64)) { \
				FactoryFuncT<class_type, class_type> factory_func = FactoryFuncImpl<class_type, class_type>; \
				return reinterpret_cast<VoidFunc>(factory_func); \
			} else if (factory_hash == Gaff::CalcTemplateHash<>(Gaff::k_init_hash64)) { \
				FactoryFuncT<class_type> factory_func = FactoryFuncImpl<class_type>; \
				return reinterpret_cast<VoidFunc>(factory_func); \
			} \
			return nullptr; \
		} \
		IReflectionStaticFunctionBase* getStaticFunc(int32_t, int32_t) const override { return nullptr; } \
		IReflectionStaticFunctionBase* getStaticFunc(Gaff::Hash32, Gaff::Hash64) const override { return nullptr; } \
		IReflectionFunctionBase* getFunc(int32_t, int32_t) const override { return nullptr; } \
		IReflectionFunctionBase* getFunc(Gaff::Hash32, Gaff::Hash64) const override { return nullptr; } \
		void* duplicate(const void*, Gaff::IAllocator&) const override { return nullptr; } \
		void destroyInstance(void* data) const override { class_type* const instance = reinterpret_cast<class_type*>(data); Gaff::Deconstruct(instance); } \
		void setAllocator(const Shibboleth::ProxyAllocator&) {} \
		void finish(void) {} \
	private: \
		Shibboleth::ProxyAllocator _allocator; \
		ptrdiff_t getBasePointerOffset(Gaff::Hash64) const override { return 0; } \
		void instantiated(void*) const override {} \
	}

// I don't like this, but these are defined in Shibboleth_ReflectionDefinition.inl to avoid some compiler errors.
//REF_DEF_BUILTIN(int8_t, Int8);
//REF_DEF_BUILTIN(int16_t, Int16);
//REF_DEF_BUILTIN(int32_t, Int32);
//REF_DEF_BUILTIN(int64_t, Int64);
//REF_DEF_BUILTIN(uint8_t, UInt8);
//REF_DEF_BUILTIN(uint16_t, UInt16);
//REF_DEF_BUILTIN(uint32_t, UInt32);
//REF_DEF_BUILTIN(uint64_t, UInt64);
//REF_DEF_BUILTIN(float, Float);
//REF_DEF_BUILTIN(double, Double);
//REF_DEF_BUILTIN(bool, Bool);

NS_END

// I don't like this, but because we hard reference built-ins (eg Reflection<bool>) in these files, we have to include it later in Shibboleth_Reflection.h
//#include "Shibboleth_ReflectionDefinition.inl"
//#include "Shibboleth_ReflectionDefinitionFunction.inl"
