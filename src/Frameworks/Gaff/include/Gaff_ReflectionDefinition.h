/************************************************************************************
Copyright (C) 2019 by Nicholas LaCroix

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

#ifdef _MSC_VER
	#pragma warning(push)
	#pragma warning(disable : 4505)
#endif

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

	bool load(const ISerializeReader& reader, void* object) const override;
	void save(ISerializeWriter& writer, const void* object) const override;
	bool load(const ISerializeReader& reader, T& object) const;
	void save(ISerializeWriter& writer, const T& object) const;

	const void* getInterface(Hash64 class_hash, const void* object) const override;
	void* getInterface(Hash64 class_hash, void* object) const override;
	bool hasInterface(Hash64 class_hash) const override;

	void setAllocator(const Allocator& allocator);

	const IReflection& getReflectionInstance(void) const override;
	int32_t size(void) const override;

	int32_t getNumVars(void) const override;
	const char* getVarName(int32_t index) const override;
	Hash32 getVarHash(int32_t index) const override;
	IReflectionVar* getVar(int32_t index) const override;
	IReflectionVar* getVar(Hash32 name) const override;

	int32_t getNumFuncs(void) const override;
	Hash32 getFuncHash(int32_t index) const override;

	int32_t getNumStaticFuncs(void) const override;
	Hash32 getStaticFuncHash(int32_t index) const override;

	int32_t getNumClassAttrs(void) const override;
	const IAttribute* getClassAttr(Hash64 attr_name) const override;
	const IAttribute* getClassAttr(int32_t index) const override;

	int32_t getNumVarAttrs(Hash32 name) const override;
	const IAttribute* getVarAttr(Hash32 name, Hash64 attr_name) const override;
	const IAttribute* getVarAttr(Hash32 name, int32_t index) const override;

	int32_t getNumFuncAttrs(Hash32 name) const override;
	const IAttribute* getFuncAttr(Hash32 name, Hash64 attr_name) const override;
	const IAttribute* getFuncAttr(Hash32 name, int32_t index) const override;

	int32_t getNumStaticFuncAttrs(Hash32 name) const override;
	const IAttribute* getStaticFuncAttr(Hash32 name, Hash64 attr_name) const override;
	const IAttribute* getStaticFuncAttr(Hash32 name, int32_t index) const override;

	VoidFunc getConstructor(Hash64 ctor_hash) const override;
	VoidFunc getFactory(Hash64 ctor_hash) const override;
	VoidFunc getStaticFunc(Hash32 name, Hash64 args) const override;
	void* getFunc(Hash32 name, Hash64 args) const override;

	void destroyInstance(void* data) const override;

	IVar* getVarT(int32_t index) const;
	IVar* getVarT(Hash32 name) const;

	const HashString32<Allocator>& getFuncName(int32_t index) const;
	const HashString32<Allocator>& getStaticFuncName(int32_t index) const;

	template <class Base>
	ReflectionDefinition& base(const char* name);

	template <class Base>
	ReflectionDefinition& base(void);

	template <class... Args>
	ReflectionDefinition& ctor(Hash64 factory_hash);

	template <class... Args>
	ReflectionDefinition& ctor(void);

	template <class Var, size_t size, class... Attrs>
	ReflectionDefinition& var(const char (&name)[size], Var T::*ptr, const Attrs&... attributes);

	template <class Ret, class Var, size_t size, class... Attrs>
	ReflectionDefinition& var(const char (&name)[size], Ret (T::*getter)(void) const, void (T::*setter)(Var), const Attrs&... attributes);

	template <class Var, class Vec_Allocator, size_t size, class... Attrs>
	ReflectionDefinition& var(const char (&name)[size], Vector<Var, Vec_Allocator> T::*vec, const Attrs&... attributes);

	template <class Var, size_t array_size, size_t name_size, class... Attrs>
	ReflectionDefinition& var(const char (&name)[name_size], Var (T::*arr)[array_size], const Attrs&... attributes);

	template <size_t size, class Ret, class... Args, class... Attrs>
	ReflectionDefinition& func(const char (&name)[size], Ret (T::*ptr)(Args...) const, const Attrs&... attributes);

	template <size_t size, class Ret, class... Args, class... Attrs>
	ReflectionDefinition& func(const char (&name)[size], Ret (T::*ptr)(Args...), const Attrs&... attributes);

	template <size_t size, class Ret, class... Args, class... Attrs>
	ReflectionDefinition& staticFunc(const char (&name)[size], Ret (*func)(Args...), const Attrs&... attributes);

	// apply() is not called on these functions. Mainly for use with the attribute file.
	template <class... Attrs>
	ReflectionDefinition& classAttrs(const Attrs&... attributes);

	ReflectionDefinition& version(uint32_t version);

	ReflectionDefinition& serialize(LoadFunc serialize_load, SaveFunc serialize_save);

	template <class T2>
	ReflectionDefinition& dependsOn(void);

	void finish(void);

private:
	template <class Var>
	class VarPtr final : public IVar
	{
	public:
		VarPtr(Var T::*ptr);

		const Gaff::IReflectionDefinition& getReflectionDefinition(void) const override;
		const void* getData(const void* object) const override;
		void* getData(void* object) override;
		void setData(void* object, const void* data) override;
		void setDataMove(void* object, void* data) override;

		void load(const ISerializeReader& reader, T& object) override;
		void save(ISerializeWriter& writer, const T& object) override;

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

		const Gaff::IReflectionDefinition& getReflectionDefinition(void) const override;
		const void* getData(const void* object) const override;
		void* getData(void* object) override;
		void setData(void* object, const void* data) override;
		void setDataMove(void* object, void* data) override;

		void load(const ISerializeReader& reader, T& object) override;
		void save(ISerializeWriter& writer, const T& object) override;

	private:
		Getter _getter = nullptr;
		Setter _setter = nullptr;

		using RetType = typename std::remove_const< typename std::remove_pointer< typename std::remove_reference<Ret>::type >::type >::type;
	};

	template <class Base>
	class BaseVarPtr final : public IVar
	{
	public:
		BaseVarPtr(typename ReflectionDefinition<Base, Allocator>::IVar* base_var);

		const Gaff::IReflectionDefinition& getReflectionDefinition(void) const override;
		const void* getData(const void* object) const override;
		void* getData(void* object) override;
		void setData(void* object, const void* data) override;
		void setDataMove(void* object, void* data) override;

		bool isFixedArray(void) const override;
		bool isVector(void) const override;
		int32_t size(const void*) const override;

		const void* getElement(const void* object, int32_t index) const override;
		void* getElement(void* object, int32_t index) override;
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
		ArrayPtr(Var (T::*ptr)[array_size]);

		const Gaff::IReflectionDefinition& getReflectionDefinition(void) const override;
		const void* getData(const void* object) const override;
		void* getData(void* object) override;
		void setData(void* object, const void* data) override;
		void setDataMove(void* object, void* data) override;

		bool isFixedArray(void) const override { return true; }
		bool isVector(void) const override { return false; }
		int32_t size(const void*) const override { return static_cast<int32_t>(array_size); }

		const void* getElement(const void* object, int32_t index) const override;
		void* getElement(void* object, int32_t index) override;
		void setElement(void* object, int32_t index, const void* data) override;
		void setElementMove(void* object, int32_t index, void* data) override;
		void swap(void* object, int32_t index_a, int32_t index_b) override;
		void resize(void* object, size_t new_size) override;

		void load(const ISerializeReader& reader, T& object) override;
		void save(ISerializeWriter& writer, const T& object) override;

	private:
		Var (T::*_ptr)[array_size] = nullptr;
	};

	template <class Var, class Vec_Allocator>
	class VectorPtr final : public IVar
	{
	public:
		VectorPtr(Vector<Var, Vec_Allocator> T::*ptr);

		const Gaff::IReflectionDefinition& getReflectionDefinition(void) const override;
		const void* getData(const void* object) const override;
		void* getData(void* object) override;
		void setData(void* object, const void* data) override;
		void setDataMove(void* object, void* data) override;

		bool isFixedArray(void) const override { return false; }
		bool isVector(void) const override { return true; }
		int32_t size(const void* object) const override;

		const void* getElement(const void* object, int32_t index) const override;
		void* getElement(void* object, int32_t index) override;
		void setElement(void* object, int32_t index, const void* data) override;
		void setElementMove(void* object, int32_t index, void* data) override;
		void swap(void* object, int32_t index_a, int32_t index_b) override;
		void resize(void* object, size_t new_size) override;

		void load(const ISerializeReader& reader, T& object) override;
		void save(ISerializeWriter& writer, const T& object) override;

	private:
		Vector<Var, Vec_Allocator> T::*_ptr = nullptr;
	};

	using IRefFuncPtr = UniquePtr<IReflectionFunctionBase, Allocator>;

	template <class Ret, class... Args>
	class ReflectionFunction final : public IReflectionFunction<Ret, Args...>
	{
	public:
		using MemFuncConst = Ret(T::*)(Args...) const;
		using MemFunc = Ret(T::*)(Args...);

		ReflectionFunction(MemFuncConst func, bool is_const) :
			_is_const(is_const)
		{
			_func.const_func = func;
		}

		ReflectionFunction(MemFunc func, bool is_const) :
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
		const IReflectionDefinition& getBaseRefDef(void) const override { return GAFF_REFLECTION_NAMESPACE::Reflection<T>::GetReflectionDefinition(); }

	private:
		union Func
		{
			MemFuncConst const_func;
			MemFunc non_const_func;
		};

		Func _func;
		bool _is_const;
	};

	class ReflectionBaseFunction final : public IReflectionFunctionBase
	{
	public:
		ReflectionBaseFunction(const IReflectionDefinition& base_ref_def, const IReflectionFunctionBase* ref_func):
			_base_ref_def(base_ref_def), _func(ref_func)
		{
		}

		bool isConst(void) const override { return _func->isConst(); }
		bool isBase(void) const override { return true; }
		const IReflectionDefinition& getBaseRefDef(void) const override { return _base_ref_def; }

		template <class Ret, class... Args>
		Ret call(const void* obj, Args... args) const
		{
			GAFF_ASSERT(isConst());

			const auto& ref_def = GAFF_REFLECTION_NAMESPACE::Reflection<T>::GetReflectionDefinition();
			const void* const object = ref_def.getBasePointer(obj, _func->getBaseRefDef().getReflectionInstance().getHash());

			if (_func->isBase()) {
				return reinterpret_cast<const ReflectionBaseFunction*>(_func)->call<Ret, Args...>(object, args...);
			} else {
				return reinterpret_cast<const ReflectionFunction<Ret, Args...>*>(_func)->call(object, args...);
			}
		}

		template <class Ret, class... Args>
		Ret call(void* obj, Args... args) const
		{
			const auto& ref_def = GAFF_REFLECTION_NAMESPACE::Reflection<T>::GetReflectionDefinition();
			void* const object = ref_def.getBasePointer(obj, _func->getBaseRefDef().getReflectionInstance().getHash());

			if (_func->isBase()) {
				return reinterpret_cast<const ReflectionBaseFunction*>(_func)->call<Ret, Args...>(object, args...);
			} else {
				return reinterpret_cast< const ReflectionFunction<Ret, Args...>* >(_func)->call(object, args...);
			}
		}

	private:
		const Gaff::IReflectionDefinition& _base_ref_def;
		const IReflectionFunctionBase* const _func;
	};

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
			return *this;
		}

		constexpr static int32_t NUM_OVERLOADS = 8;
		Hash64 hash[NUM_OVERLOADS] = { 0 };
		IRefFuncPtr func[NUM_OVERLOADS];
	};

	struct StaticFuncData
	{
		StaticFuncData(void)
		{
			memset(func, 0, sizeof(VoidFunc) * NUM_OVERLOADS);
		}

		StaticFuncData(const StaticFuncData& rhs)
		{
			*this = rhs;
		}

		StaticFuncData& operator=(const StaticFuncData& rhs)
		{
			memcpy(func, rhs.func, sizeof(VoidFunc) * NUM_OVERLOADS);
			memcpy(hash, rhs.hash, sizeof(Hash64) * NUM_OVERLOADS);
			return *this;
		}

		template <class T2, class A2>
		typename ReflectionDefinition<T2, A2>::StaticFuncData toDerived(void) const
		{
			typename ReflectionDefinition<T2, A2>::StaticFuncData func_data;
			memcpy(func_data.func, func, sizeof(VoidFunc) * NUM_OVERLOADS);
			memcpy(func_data.hash, hash, sizeof(Hash64) * NUM_OVERLOADS);
			return func_data;
		}

		constexpr static int32_t NUM_OVERLOADS = 8;
		Hash64 hash[NUM_OVERLOADS] = { 0 };
		VoidFunc func[NUM_OVERLOADS] = { nullptr };
	};

	VectorMap<HashString64<Allocator>, ptrdiff_t, Allocator> _base_class_offsets;
	VectorMap<HashString32<Allocator>, IVarPtr, Allocator> _vars;
	VectorMap<HashString32<Allocator>, FuncData, Allocator> _funcs;
	VectorMap<HashString32<Allocator>, StaticFuncData, Allocator> _static_funcs;
	VectorMap<Hash64, VoidFunc, Allocator> _factories;
	VectorMap<Hash64, VoidFunc, Allocator> _ctors;
	VectorMap<Hash64, const IReflectionDefinition*, Allocator> _base_classes;

	using AttributeList = Vector<IAttributePtr, Allocator>;

	VectorMap<Hash32, AttributeList, Allocator> _var_attrs;
	VectorMap<Hash64, AttributeList, Allocator> _func_attrs;
	VectorMap<Hash64, AttributeList, Allocator> _static_func_attrs;
	AttributeList _class_attrs;

	LoadFunc _serialize_load = nullptr;
	SaveFunc _serialize_save = nullptr;

	mutable Allocator _allocator;

	int32_t _dependents_remaining = 0;

	template <class Base>
	static void RegisterBaseVariables(void);

	static void FinishAfterDependent(void);

	// Variables
	template <class Var, class First, class... Rest>
	ReflectionDefinition& addAttributes(Gaff::IReflectionVar* ref_var, Var T::*var, Vector<IAttributePtr, Allocator>& attrs, const First& first, const Rest&... rest);
	template <class Var, class Ret, class First, class... Rest>
	ReflectionDefinition& addAttributes(Gaff::IReflectionVar* ref_var, Ret (T::*getter)(void) const, void (T::*setter)(Var), Vector<IAttributePtr, Allocator>& attrs, const First& first, const Rest&... rest);

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

	const IAttribute* getAttribute(const AttributeList& attributes, Gaff::Hash64 attr_name) const;

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
		const IReflection& getReflectionInstance(void) const override { return GAFF_REFLECTION_NAMESPACE::Reflection<class_type>::GetInstance(); } \
		int32_t size(void) const override { return sizeof(class_type); } \
		bool load(const ISerializeReader& reader, void* object) const override { return load(reader, *reinterpret_cast<class_type*>(object)); } \
		void save(ISerializeWriter& writer, const void* object) const override { save(writer, *reinterpret_cast<const class_type*>(object)); } \
		bool load(const ISerializeReader& reader, class_type& out) const { out = reader.read##serialize_type(); return true; } \
		void save(ISerializeWriter& writer, const class_type& value) const { writer.write##serialize_type(value); } \
		const void* getInterface(Hash64, const void*) const override { return nullptr; } \
		void* getInterface(Hash64, void*) const override { return nullptr; } \
		bool hasInterface(Hash64) const override { return false; } \
		int32_t getNumVars(void) const override { return 0; } \
		const char* getVarName(int32_t) const override { return nullptr; } \
		Hash32 getVarHash(int32_t) const override { return 0; } \
		IReflectionVar* getVar(int32_t) const override { return nullptr; } \
		IReflectionVar* getVar(Hash32) const override { return nullptr; } \
		int32_t getNumFuncs(void) const override { return 0; } \
		Hash32 getFuncHash(int32_t) const override { return 0; } \
		int32_t getNumStaticFuncs(void) const override { return 0; } \
		Hash32 getStaticFuncHash(int32_t) const override { return 0; } \
		int32_t getNumClassAttrs(void) const override { return 0; } \
		const IAttribute* getClassAttr(Hash64) const override { return nullptr; } \
		const IAttribute* getClassAttr(int32_t) const override { return nullptr; } \
		int32_t getNumVarAttrs(Hash32) const override { return 0; } \
		const IAttribute* getVarAttr(Hash32, Hash64) const { return nullptr; } \
		const IAttribute* getVarAttr(Hash32, int32_t) const override { return nullptr; } \
		int32_t getNumFuncAttrs(Hash32) const override { return 0; } \
		const IAttribute* getFuncAttr(Hash32, Hash64) const { return nullptr; } \
		const IAttribute* getFuncAttr(Hash32, int32_t) const override { return nullptr; } \
		int32_t getNumStaticFuncAttrs(Hash32) const override { return 0; } \
		const IAttribute* getStaticFuncAttr(Hash32, Hash64) const { return nullptr; } \
		const IAttribute* getStaticFuncAttr(Hash32, int32_t) const override { return nullptr; } \
		VoidFunc getConstructor(Hash64 ctor_hash) const override \
		{ \
			if (ctor_hash == CalcTemplateHash<class_type>(INIT_HASH64)) { \
				ConstructFunc<class_type> construct_func = Gaff::ConstructFunc<class_type, class_type>; \
				return reinterpret_cast<VoidFunc>(construct_func); \
			} else if (ctor_hash == CalcTemplateHash<>(INIT_HASH64)) { \
				ConstructFunc<> construct_func = Gaff::ConstructFunc<class_type>; \
				return reinterpret_cast<VoidFunc>(construct_func); \
			} \
			return nullptr; \
		} \
		VoidFunc getFactory(Hash64 factory_hash) const override \
		{ \
			if (factory_hash == CalcTemplateHash<class_type>(INIT_HASH64)) { \
				FactoryFunc<class_type> factory_func = Gaff::FactoryFunc<class_type, class_type>; \
				return reinterpret_cast<VoidFunc>(factory_func); \
			} else if (factory_hash == CalcTemplateHash<>(INIT_HASH64)) { \
				FactoryFunc<> factory_func = Gaff::FactoryFunc<class_type>; \
				return reinterpret_cast<VoidFunc>(factory_func); \
			} \
			return nullptr; \
		} \
		VoidFunc getStaticFunc(Hash32, Hash64) const override { return nullptr; } \
		void* getFunc(Hash32, Hash64) const override { return nullptr; } \
		void destroyInstance(void* data) const override { class_type* const instance = reinterpret_cast<class_type*>(data); Gaff::Deconstruct(instance); } \
		void setAllocator(const Allocator&) {} \
		void finish(void) {} \
	private: \
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

NS_END

#include "Gaff_ReflectionDefinition.inl"

#define CLASS_HASH(Class) Gaff::FNV1aHash64Const(#Class)
#define ARG_HASH(...) Gaff::CalcTemplateHash<__VA_ARGS__>(Gaff::INIT_HASH64, eastl::array<const char*, Gaff::GetNumArgs<__VA_ARGS__>()>{ GAFF_FOR_EACH_COMMA(GAFF_STR, __VA_ARGS__) })
#define BASE(type) template base<type>(#type)
#define CTOR(...) ctor<__VA_ARGS__>(ARG_HASH(__VA_ARGS__))
#define GET_INTERFACE(Class, data) getInterface<Class>(CLASS_HASH(Class), data)

#ifdef _MSC_VER
	#pragma warning(pop)
#endif
