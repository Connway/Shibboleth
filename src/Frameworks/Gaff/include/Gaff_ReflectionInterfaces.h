/************************************************************************************
Copyright (C) 2020 by Nicholas LaCroix

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

#include "Gaff_SerializeInterfaces.h"
#include "Gaff_Vector.h"
#include "Gaff_Assert.h"
#include "Gaff_Flags.h"
#include "Gaff_Hash.h"
#include <EASTL/array.h>

#define GET_CLASS_ATTRS(T, Allocator) getClassAttrs<T, Allocator>(Gaff::FNV1aHash64Const(#T))
#define GET_CLASS_ATTR(T) getClassAttr<T>(Gaff::FNV1aHash64Const(#T))
#define GET_VAR_ATTR(T, var_name) getVarAttr<T>(var_name, Gaff::FNV1aHash64Const(#T))
#define GET_FUNC_ATTR(T, func_name) getFuncAttr<T>(func_name, Gaff::FNV1aHash64Const(#T))
#define GET_STATIC_FUNC_ATTR(T, static_func_name) getStaticFuncAttr<T>(static_func_name, Gaff::FNV1aHash64Const(#T))
#define GET_ENUM_ATTR(T) getEnumAttr<T>(Gaff::FNV1aHash64Const(#T))
#define GET_ENUM_VALUE_ATTR(T, value_name) getEnumValueAttr<T>(value_name, Gaff::FNV1aHash64Const(#T))

#define INTERFACE_CAST(interface_type, object) Gaff::InterfaceCast<interface_type>(object, Gaff::FNV1aHash64Const(#interface_type))

#ifndef GAFF_REFLECTION_NAMESPACE
	#define GAFF_REFLECTION_NAMESPACE Gaff
#endif

#ifndef NS_REFLECTION
	#define NS_REFLECTION namespace GAFF_REFLECTION_NAMESPACE {
#endif

NS_GAFF

class IEnumReflectionDefinition;
class IReflectionDefinition;
class IAllocator;
struct FunctionStackEntry;

class IReflection
{
public:
	virtual ~IReflection(void) {}

	virtual bool isEnum(void) const = 0;

	virtual void init(void) = 0;

	virtual const char* getName(void) const = 0;
	virtual Hash64 getHash(void) const = 0;
	virtual Hash64 getVersion(void) const = 0;
	virtual int32_t size(void) const = 0;

	virtual const IEnumReflectionDefinition& getEnumReflectionDefinition(void) const
	{
		GAFF_ASSERT_MSG(false, "Is a class reflected type!");
		const IEnumReflectionDefinition* const ptr = nullptr;
		return *ptr;
	}

	virtual const IReflectionDefinition& getReflectionDefinition(void) const
	{
		GAFF_ASSERT_MSG(false, "Is an enum reflected type!");
		const IReflectionDefinition* const ptr = nullptr;
		return *ptr;
	}

	IReflection* next = nullptr;
};

NS_END

NS_REFLECTION

template <class T>
class Reflection final : public Gaff::IReflection
{
public:
	constexpr static bool HasReflection = false;

	Reflection(void)
	{
		GAFF_ASSERT_MSG(false, "Unknown object type.");
	}

	bool isEnum(void) const override
	{
		GAFF_ASSERT_MSG(false, "Unknown object type.");
		return false;
	}

	void init(void) override
	{
		GAFF_ASSERT_MSG(false, "Unknown object type.");
	}

	const char* getName(void) const override
	{
		GAFF_ASSERT_MSG(false, "Unknown object type.");
		return "Unknown";
	}

	Gaff::Hash64 getHash(void) const override
	{
		GAFF_ASSERT_MSG(false, "Unknown object type.");
		return 0;
	}

	Gaff::Hash64 getVersion(void) const override
	{
		GAFF_ASSERT_MSG(false, "Unknown object type.");
		return 0;
	}
};

NS_END


NS_GAFF

class IReflectionObject;

//template <class T>
//const T* ReflectionCast(const IReflectionObject& object);
//
//template <class T>
//T* ReflectionCast(IReflectionObject& object);

//template <class T>
//const T* InterfaceCast(const IReflectionObject& object, Hash64 interface_name);
//
//template <class T>
//T* InterfaceCast(IReflectionObject& object, Hash64 interface_name);

class IReflectionObject
{
public:
	virtual ~IReflectionObject(void) {}

	virtual const IReflectionDefinition& getReflectionDefinition(void) const = 0;

	virtual const void* getBasePointer(void) const = 0;
	virtual void* getBasePointer(void) = 0;
};

class IReflectionVar
{
public:
	virtual ~IReflectionVar(void) {}

	template <class DataType>
	void setDataT(void* object, const DataType& data)
	{
		if (isReadOnly()) {
			// $TODO: Log error.
			return;
		}

		using Type = typename std::remove_reference<DataType>::type;

		const auto& other_refl = GAFF_REFLECTION_NAMESPACE::Reflection<Type>::GetInstance();
		const auto& refl = getReflection();

		GAFF_ASSERT(refl.isEnum() == other_refl.isEnum());

		if constexpr (std::is_enum<Type>::value) {
			GAFF_ASSERT(&refl.getEnumReflectionDefinition() == &other_refl.getEnumReflectionDefinition());
		} else {
			GAFF_ASSERT(&refl.getReflectionDefinition() == &other_refl.getReflectionDefinition());
		}

		setData(object, &data);
	}

	template <class DataType>
	void setDataMoveT(void* object, DataType&& data)
	{
		if (isReadOnly()) {
			// $TODO: Log error.
			return;
		}

		using Type = typename std::remove_reference<DataType>::type;

		const auto& other_refl = GAFF_REFLECTION_NAMESPACE::Reflection<Type>::GetInstance();
		const auto& refl = getReflection();

		GAFF_ASSERT(refl.isEnum() == other_refl.isEnum());

		if constexpr (std::is_enum<Type>::value) {
			GAFF_ASSERT(&refl.getEnumReflectionDefinition() == &other_refl.getEnumReflectionDefinition());
		} else {
			GAFF_ASSERT(&refl.getReflectionDefinition() == &other_refl.getReflectionDefinition());
		}

		setDataMove(object, &data);
	}

	template <class DataType>
	const DataType& getElementT(const void* object, int32_t index) const
	{
		using Type = typename std::remove_reference<DataType>::type;

		const auto& other_refl = GAFF_REFLECTION_NAMESPACE::Reflection<Type>::GetInstance();
		const auto& refl = getReflection();

		GAFF_ASSERT(refl.isEnum() == other_refl.isEnum());

		if constexpr (std::is_enum<Type>::value) {
			GAFF_ASSERT(&refl.getEnumReflectionDefinition() == &other_refl.getEnumReflectionDefinition());
		} else {
			GAFF_ASSERT(&refl.getReflectionDefinition() == &other_refl.getReflectionDefinition());
		}

		GAFF_ASSERT((isFixedArray() || isVector()) && size(object) > index);
		return *reinterpret_cast<const DataType*>(getElement(object, index));
	}

	template <class DataType>
	void setElementT(void* object, int32_t index, const DataType& data)
	{
		if (isReadOnly()) {
			// $TODO: Log error.
			return;
		}

		using Type = typename std::remove_reference<DataType>::type;

		const auto& other_refl = GAFF_REFLECTION_NAMESPACE::Reflection<Type>::GetInstance();
		const auto& refl = getReflection();

		GAFF_ASSERT(refl.isEnum() == other_refl.isEnum());

		if constexpr (std::is_enum<Type>::value) {
			GAFF_ASSERT(&refl.getEnumReflectionDefinition() == &other_refl.getEnumReflectionDefinition());
		} else {
			GAFF_ASSERT(&refl.getReflectionDefinition() == &other_refl.getReflectionDefinition());
		}

		GAFF_ASSERT((isFixedArray() || isVector()) && size(object) > index);
		setElement(object, index, &data);
	}

	template <class DataType>
	void setElementMoveT(void* object, int32_t index, DataType&& data)
	{
		if (isReadOnly()) {
			// $TODO: Log error.
			return;
		}

		using Type = typename std::remove_reference<DataType>::type;

		const auto& other_refl = GAFF_REFLECTION_NAMESPACE::Reflection<Type>::GetInstance();
		const auto& refl = getReflection();

		GAFF_ASSERT(refl.isEnum() == other_refl.isEnum());

		if constexpr (std::is_enum<Type>::value) {
			GAFF_ASSERT(&refl.getEnumReflectionDefinition() == &other_refl.getEnumReflectionDefinition());
		} else {
			GAFF_ASSERT(&refl.getReflectionDefinition() == &other_refl.getReflectionDefinition());
		}

		GAFF_ASSERT((isFixedArray() || isVector()) && size(object) > index);
		setElementMove(object, index, &data);
	}

	virtual const IReflection& getReflection(void) const = 0;
	virtual const void* getData(const void* object) const = 0;
	virtual void* getData(void* object) = 0;
	virtual void setData(void* object, const void* data) = 0;
	virtual void setDataMove(void* object, void* data) = 0;

	virtual bool isFixedArray(void) const { return false; }
	virtual bool isVector(void) const { return false; }
	virtual bool isFlags(void) const { return false; }
	virtual bool isMap(void) const { return false; }

	virtual const IReflection& getReflectionKey(void) const
	{
		GAFF_ASSERT_MSG(false, "Reflection variable is not a vector map!");
		return *static_cast<IReflection*>(nullptr);
	}

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

	virtual void* getElement(void*, int32_t)
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

	void setReadOnly(bool read_only) { _flags.set(read_only, Flag::ReadOnly); }
	bool isReadOnly(void) const { return _flags.testAll(Flag::ReadOnly); }

	void setNoSerialize(bool no_serialize) { _flags.set(no_serialize, Flag::NoSerialize); }
	bool canSerialize(void) const { return !_flags.testAll(Flag::NoSerialize); }

private:
	enum class Flag
	{
		NoSerialize,
		ReadOnly,

		Count
	};

	Flags<Flag> _flags;
};

class IAttribute : public IReflectionObject
{
public:
	virtual IAttribute* clone(void) const = 0;

	virtual Hash64 applyVersioning(Hash64 hash) const { return hash; }
	virtual bool canInherit(void) const { return true; }

	virtual void finish(const IReflectionDefinition& /*ref_def*/) {}
	virtual void finish(const IEnumReflectionDefinition& /*ref_def*/) {}

	virtual void instantiated(const IReflectionDefinition& /*ref_def*/, void* /*object*/) {}

	// The apply function corresponds directly to calls in reflection definition. Apply all that apply.

	// Attributes that are applied to functions need to implement these template functions.
	template <class T, class Ret, class... Args>
	void apply(Ret (T::* /*func*/)(Args...) const) {}
	template <class T, class Ret, class... Args>
	void apply(Ret (T::* /*func*/)(Args...)) {}

	// Attributes that are applied to static class functions need to implement this template function.
	template <class T, class Ret, class... Args>
	void apply(Ret (* /*func*/)(Args...)) {}

	// Attributes that are applied to variables need to implement these template functions,
	// or at least the ones they apply to.
	template <class T, class Var>
	void apply(IReflectionVar& /*ref_var*/, Var T::* /*var*/) {}
	template <class T, class Var, class Ret>
	void apply(IReflectionVar& /*ref_var*/, Ret (T::* /*getter*/)(void) const, void (T::* /*setter*/)(Var)) {}
	template <class T, class Var, class Vec_Allocator, size_t size>
	void apply(IReflectionVar& /*ref_var*/, Vector<Var, Vec_Allocator> T::* /*vec*/) {}
	template <class T, class Var, size_t size>
	void apply(IReflectionVar& /*ref_var*/, Var (T::* /*arr*/)[size]) {}
};

class IReflectionFunctionBase
{
public:
	IReflectionFunctionBase(void) {}
	virtual ~IReflectionFunctionBase(void) {}

	virtual bool call(const void* object, const FunctionStackEntry* args, int32_t num_args, FunctionStackEntry& ret, IAllocator& allocator) const = 0;
	virtual bool call(void* object, const FunctionStackEntry* args, int32_t num_args, FunctionStackEntry& ret, IAllocator& allocator) const = 0;

	virtual int32_t numArgs(void) const = 0;
	virtual bool isConst(void) const = 0;
	virtual bool isBase(void) const { return false; }
	virtual const IReflectionDefinition& getBaseRefDef(void) const = 0;
};

template <class Ret, class... Args>
class IReflectionFunction : public IReflectionFunctionBase
{
public:
	int32_t numArgs(void) const override { return static_cast<int32_t>(sizeof...(Args)); }

	virtual Ret call(const void* object, Args&&... args) const = 0;
	virtual Ret call(void* object, Args&&... args) const = 0;
};

class IReflectionStaticFunctionBase
{
public:
	using VoidFunc = void (*)(void);

	explicit IReflectionStaticFunctionBase(VoidFunc func):
		_func(func)
	{
	}

	virtual ~IReflectionStaticFunctionBase(void) {}

	VoidFunc getFunc(void) const { return _func; }

	virtual bool call(const FunctionStackEntry* args, int32_t num_args, FunctionStackEntry& ret, IAllocator& allocator) const = 0;

	virtual int32_t numArgs(void) const = 0;
	virtual IReflectionStaticFunctionBase* clone(IAllocator& allocator) const = 0;

protected:
	VoidFunc _func = nullptr;
};

template <class Ret, class... Args>
class IReflectionStaticFunction : public IReflectionStaticFunctionBase
{
public:
	using Func = Ret (*)(Args...);

	explicit IReflectionStaticFunction(Func func) :
		IReflectionStaticFunctionBase(reinterpret_cast<VoidFunc>(func))
	{
	}

	Func getFunc(void) const { return reinterpret_cast<Func>(_func); }

	Ret call(Args&&... args) const
	{
		return reinterpret_cast<Func>(_func)(std::forward<Args>(args)...);
	}
};


#define CREATET(Class, allocator, ...) template createT<Class>(Gaff::FNV1aHash64Const(#Class), allocator ##__VA_ARGS__)

struct FunctionStackEntry final
{
	enum class Flag
	{
		IsReference,
		IsString,
		IsArray,
		IsMap,

		Count
	};

	struct ArrayData final
	{
		void* data;
		int32_t size;
	};

	union Value
	{
		ArrayData arr;
		void* vp;
		bool b;
		int8_t i8;
		int16_t i16;
		int32_t i32;
		int64_t i64;
		uint8_t u8;
		uint16_t u16;
		uint32_t u32;
		uint64_t u64;
		float f;
		double d;
	};

	FunctionStackEntry(void)
	{
		// Zero out everything.
		value.arr.data = nullptr;
		value.arr.size = 0;
	}

	const IEnumReflectionDefinition* enum_ref_def = nullptr;
	const IReflectionDefinition* ref_def = nullptr;
	Flags<Flag> flags;
	Value value;
};

class IReflectionDefinition
{
public:
	template <class T, class... Args>
	using ConstructFuncT = void (*)(T*, Args&&...);

	template <class... Args>
	using ConstructFunc = void (*)(void*, Args&&...);

	template <class T, class... Args>
	using FactoryFuncT = T* (*)(IAllocator&, Args&&...);

	template <class... Args>
	using FactoryFunc = void* (*)(IAllocator&, Args&&...);

	template <class Ret, class... Args>
	using TemplateFunc = Ret (*)(Args...);

	using VoidFunc = void (*)(void);

	const void* getBasePointer(Hash64 interface_name, const void* object) const
	{
		return const_cast<IReflectionDefinition*>(this)->getBasePointer(interface_name, const_cast<void*>(object));
	}

	void* getBasePointer(Hash64 interface_name, void* object) const
	{
		const ptrdiff_t offset = getBasePointerOffset(interface_name);
		return reinterpret_cast<int8_t*>(object) - offset;
	}

	template <class T>
	const void* getBasePointer(const T* object) const
	{
		return getBasePointer(GAFF_REFLECTION_NAMESPACE::Reflection<T>::GetHash(), object);
	}

	template <class T>
	void* getBasePointer(T* object) const
	{
		return getBasePointer(GAFF_REFLECTION_NAMESPACE::Reflection<T>::GetHash(), object);
	}

	template <class T>
	const T* getInterface(const void* object) const
	{
		return reinterpret_cast<const T*>(getInterface(GAFF_REFLECTION_NAMESPACE::Reflection<T>::GetHash(), object));
	}

	template <class T>
	T* getInterface(void* object) const
	{
		return reinterpret_cast<T*>(getInterface(GAFF_REFLECTION_NAMESPACE::Reflection<T>::GetHash(), object));
	}

	template <class T>
	const T* getInterface(Hash64 interface_hash, const void* object) const
	{
		return reinterpret_cast<const T*>(getInterface(interface_hash, object));
	}

	template <class T>
	T* getInterface(Hash64 interface_hash, void* object) const
	{
		return reinterpret_cast<T*>(getInterface(interface_hash, object));
	}

	template <class T, class... Args>
	T* createT(Hash64 interface_hash, Hash64 factory_hash, IAllocator& allocator, Args&&... args) const
	{
		GAFF_ASSERT(hasInterface(interface_hash));
		void* const data = create(factory_hash, allocator, std::forward<Args>(args)...);

		if (data) {
			return reinterpret_cast<T*>(getInterface(interface_hash, data));
		}

		return nullptr;
	}

	template <class T, class... Args>
	T* createT(Hash64 interface_hash, IAllocator& allocator, Args&&... args) const
	{
		constexpr Hash64 ctor_hash = CalcTemplateHash<Args...>(INIT_HASH64);
		return createT<T>(interface_hash, ctor_hash, allocator, std::forward<Args>(args)...);
	}

	template <class T, class... Args>
	T* createT(IAllocator& allocator, Args&&... args) const
	{
		constexpr Hash64 interface_hash = GAFF_REFLECTION_NAMESPACE::Reflection<T>::GetHash();
		constexpr Hash64 ctor_hash = CalcTemplateHash<Args...>(INIT_HASH64);
		return createT<T>(interface_hash, ctor_hash, allocator, std::forward<Args>(args)...);
	}

	template <class... Args>
	void* create(Hash64 factory_hash, IAllocator& allocator, Args&&... args) const
	{
		FactoryFunc<Args...> factory_func = reinterpret_cast< FactoryFunc<Args...> >(getFactory(factory_hash));

		if (factory_func) {
			void* const object = factory_func(allocator, std::forward<Args>(args)...);
			instantiated(object);
			return object;
		}

		return nullptr;
	}

	template <class... Args>
	void* create(IAllocator& allocator, Args&&... args) const
	{
		constexpr Hash64 ctor_hash = CalcTemplateHash<Args...>(INIT_HASH64);
		return create(ctor_hash, allocator, std::forward<Args>(args)...);
	}

	template <class T>
	const T* getClassAttr(Hash64 attr_name) const
	{
		const auto* const attr = getClassAttr(attr_name);
		return (attr) ? static_cast<const T*>(attr) : nullptr;
	}

	template <class T>
	const T* getVarAttr(Hash32 var_name, Hash64 attr_name) const
	{
		const auto* const attr = getVarAttr(var_name, attr_name);
		return (attr) ? static_cast<const T*>(attr) : nullptr;
	}

	template <class T>
	const T* getFuncAttr(Hash32 func_name, Hash64 attr_name) const
	{
		const auto* const attr = getFuncAttr(func_name, attr_name);
		return (attr) ? static_cast<const T*>(attr) : nullptr;
	}

	template <class T>
	const T* getStaticFuncAttr(Hash32 static_func_name, Hash64 attr_name) const
	{
		const auto* const attr = getStaticFuncAttr(static_func_name, attr_name);
		return (attr) ? static_cast<const T*>(attr) : nullptr;
	}

	template <class T>
	const T* getClassAttr(void) const
	{
		return getClassAttr<T>( GAFF_REFLECTION_NAMESPACE::Reflection<T>::GetHash());
	}

	template <class T>
	const T* getVarAttr(Hash32 name) const
	{
		return getVarAttr<T>(name, GAFF_REFLECTION_NAMESPACE::Reflection<T>::GetHash());
	}

	template <class T>
	const T* getFuncAttr(Hash32 name) const
	{
		return getFuncAttr<T>(name, GAFF_REFLECTION_NAMESPACE::Reflection<T>::GetHash());
	}

	template <class T>
	const T* getStaticFuncAttr(Hash32 name) const
	{
		return getStaticFuncAttr<T>(name, GAFF_REFLECTION_NAMESPACE::Reflection<T>::GetHash());
	}

	template <class T>
	eastl::pair<Hash32, const T*> getVarAttr(void) const
	{
		const int32_t num_vars = getNumVars();

		for (int32_t j = 0; j < num_vars; ++j) {
			const Hash32 name = getVarHash(j);
			const int32_t size = getNumVarAttrs(name);

			for (int32_t i = 0; i < size; ++i) {
				const IAttribute* const attribute = getVarAttr(name, i);
				const T* attr = ReflectionCast<T>(*attribute);

				if (attr) {
					return eastl::make_pair(name, attr);
				}
			}
		}

		return eastl::pair<Hash32, const T*>(0, nullptr);
	}

	template <class T>
	eastl::pair<Hash32, const T*> getVarAttr(Hash64 attr_name) const
	{
		const int32_t num_vars = getNumVars();

		for (int32_t j = 0; j < num_vars; ++j) {
			const Hash32 name = getVarHash(j);
			const int32_t size = getNumVarAttrs(name);

			for (int32_t i = 0; i < size; ++i) {
				const IAttribute* const attribute = getVarAttr(name, i);
				const void* attr = attribute->getReflectionDefinition().getInterface(
					attr_name, attribute->getBasePointer()
				);

				if (attr) {
					return eastl::make_pair(name, reinterpret_cast<const T*>(attr));
				}
			}
		}

		return eastl::pair<Hash32, const T*>(0, nullptr);
	}

	template <class T>
	eastl::pair<Hash32, const T*> getFuncAttr(void) const
	{
		const int32_t num_vars = getNumFuncs();

		for (int32_t j = 0; j < num_vars; ++j) {
			const Hash32 name = getFuncHash(j);
			const int32_t size = getNumFuncAttrs(name);

			for (int32_t i = 0; i < size; ++i) {
				const IAttribute* const attribute = getFuncAttr(name, i);
				const T* attr = ReflectionCast<T>(*attribute);

				if (attr) {
					return eastl::make_pair(name, attr);
				}
			}
		}

		return eastl::pair<Hash32, const T*>(0, nullptr);
	}

	template <class T>
	eastl::pair<Hash32, const T*> getFuncAttr(Hash64 attr_name) const
	{
		const int32_t num_vars = getNumFuncs();

		for (int32_t j = 0; j < num_vars; ++j) {
			const Hash32 name = getFuncHash(j);
			const int32_t size = getNumFuncAttrs(name);

			for (int32_t i = 0; i < size; ++i) {
				const IAttribute* const attribute = getFuncAttr(name, i);
				const void* attr = attribute->getReflectionDefinition().getInterface(
					attr_name, attribute->getBasePointer()
				);

				if (attr) {
					return eastl::make_pair(name, reinterpret_cast<const T*>(attr));
				}
			}
		}

		return eastl::pair<Hash32, const T*>(0, nullptr);
	}

	template <class T>
	eastl::pair<Hash32, const T*> getStaticFuncAttr(void) const
	{
		const int32_t num_vars = getNumStaticFuncs();

		for (int32_t j = 0; j < num_vars; ++j) {
			const Hash32 name = getStaticFuncHash(j);
			const int32_t size = getNumStaticFuncAttrs(name);

			for (int32_t i = 0; i < size; ++i) {
				const IAttribute* const attribute = getStaticFuncAttr(name, i);
				const T* attr = ReflectionCast<T>(*attribute);

				if (attr) {
					return eastl::make_pair(name, attr);
				}
			}
		}

		return eastl::pair<Hash32, const T*>(0, nullptr);
	}

	template <class T>
	eastl::pair<Hash32, const T*> getStaticFuncAttr(Hash64 attr_name) const
	{
		const int32_t num_vars = getNumStaticFuncs();

		for (int32_t j = 0; j < num_vars; ++j) {
			const Hash32 name = getStaticFuncHash(j);
			const int32_t size = getNumStaticFuncAttrs(name);

			for (int32_t i = 0; i < size; ++i) {
				const IAttribute* const attribute = getStaticFuncAttr(name, i);
				const void* attr = attribute->getReflectionDefinition().getInterface(
					attr_name, attribute->getBasePointer()
				);

				if (attr) {
					return eastl::make_pair(name, reinterpret_cast<const T*>(attr));
				}
			}
		}

		return eastl::pair<Hash32, const T*>(0, nullptr);
	}

	template <class T, class Allocator>
	void getClassAttrs(Vector<const T*, Allocator>& out) const
	{
		for (int32_t i = 0; i < getNumClassAttrs(); ++i) {
			const IAttribute* const attribute = getClassAttr(i);
			const T* attr = ReflectionCast<T>(*attribute);

			if (attr) {
				out.emplace_back(attr);
			}
		}
	}

	template <class T, class Allocator>
	void getClassAttrs(Hash64 attr_name, Vector<const T*, Allocator>& out) const
	{
		for (int32_t i = 0; i < getNumClassAttrs(); ++i) {
			const IAttribute* const attribute = getClassAttr(i);
			const void* attr = attribute->getReflectionDefinition().getInterface(
				attr_name, attribute->getBasePointer()
			);

			if (attr) {
				out.emplace_back(reinterpret_cast<const T*>(attr));
			}
		}
	}

	template <class T, class Allocator>
	Vector<const T*, Allocator> getClassAttrs(Hash64 attr_name) const
	{
		Vector<const T*, Allocator> out;
		getClassAttrs(attr_name, out);

		return out;
	}

	template <class T, class Allocator>
	void getVarAttrs(Hash32 name, Vector<const T*, Allocator>& out) const
	{
		const int32_t size = getNumVarAttrs(name);

		for (int32_t i = 0; i < size; ++i) {
			const IAttribute* const attribute = getVarAttr(name, i);
			const T* attr = ReflectionCast<T>(*attribute);

			if (attr) {
				out.emplace_back(attr);
			}
		}
	}

	template <class T, class Allocator>
	void getVarAttrs(Vector<eastl::pair<Hash32, const T&>, Allocator>& out) const
	{
		const int32_t num_vars = getNumVars();

		for (int32_t j = 0; j < num_vars; ++j) {
			const Hash32 name = getVarHash(j);
			const int32_t size = getNumVarAttrs(name);

			for (int32_t i = 0; i < size; ++i) {
				const IAttribute* const attribute = getVarAttr(name, i);
				const T* attr = ReflectionCast<T>(*attribute);

				if (attr) {
					out.emplace_back(eastl::make_pair(name, attr));
				}
			}
		}
	}

	template <class T, class Allocator>
	void getFuncAttrs(Hash32 name, Vector<const T*, Allocator>& out) const
	{
		const int32_t size = getNumFuncAttrs(name);

		for (int32_t i = 0; i < size; ++i) {
			const IAttribute* const attribute = getFuncAttr(name, i);
			const T* attr = ReflectionCast<T>(*attribute);

			if (attr) {
				out.emplace_back(attr);
			}
		}
	}

	template <class T, class Allocator>
	void getFuncAttrs(Vector<eastl::pair<Hash32, const T&>, Allocator>& out) const
	{
		const int32_t num_vars = getNumFuncs();

		for (int32_t j = 0; j < num_vars; ++j) {
			const Hash32 name = getFuncHash(j);
			const int32_t size = getNumFuncAttrs(name);

			for (int32_t i = 0; i < size; ++i) {
				const IAttribute* const attribute = getFuncAttr(name, i);
				const T* attr = ReflectionCast<T>(*attribute);

				if (attr) {
					out.emplace_back(eastl::make_pair(name, attr));
				}
			}
		}
	}

	template <class T, class Allocator>
	void getStaticFuncAttrs(Hash32 name, Vector<const T*, Allocator>& out) const
	{
		const int32_t size = getNumStaticFuncAttrs(name);

		for (int32_t i = 0; i < size; ++i) {
			const IAttribute* const attribute = getStaticFuncAttr(name, i);
			const T* attr = ReflectionCast<T>(*attribute);

			if (attr) {
				out.emplace_back(attr);
			}
		}
	}

	template <class T, class Allocator>
	void getStaticFuncAttrs(Vector<eastl::pair<Hash32, const T&>, Allocator>& out) const
	{
		const int32_t num_vars = getNumFuncs();

		for (int32_t j = 0; j < num_vars; ++j) {
			const Hash32 name = getStaticFuncHash(j);
			const int32_t size = getNumStaticFuncAttrs(name);

			for (int32_t i = 0; i < size; ++i) {
				const IAttribute* const attribute = getStaticFuncAttr(name, i);
				const T* attr = ReflectionCast<T>(*attribute);

				if (attr) {
					out.emplace_back(eastl::make_pair(name, attr));
				}
			}
		}
	}

	template <class... Args>
	ConstructFunc<Args...> getConstructor(void) const
	{
		constexpr Hash64 ctor_hash = CalcTemplateHash<Args...>(INIT_HASH64);
		return reinterpret_cast<ConstructFunc<Args...>>(getConstructor(ctor_hash));
	}

	template <class... Args>
	FactoryFunc<Args...> getFactory(void) const
	{
		constexpr Hash64 ctor_hash = CalcTemplateHash<Args...>(INIT_HASH64);
		return reinterpret_cast< FactoryFunc<Args...> >(getFactory(ctor_hash));
	}

	template <class Ret, class... Args>
	IReflectionFunction<Ret, Args...>* getFunc(Hash32 name) const
	{
		constexpr Hash64 arg_hash = CalcTemplateHash<Ret, Args...>(INIT_HASH64);
		IReflectionFunctionBase* const functor = getFunc(name, arg_hash);

		if (functor) {
			return static_cast<IReflectionFunction<Ret, Args...>*>(functor);
		}

		return nullptr;
	}

	template <class Ret, class... Args>
	IReflectionStaticFunction<Ret, Args...>* getStaticFunc(Hash32 name) const
	{
		constexpr Hash64 arg_hash = CalcTemplateHash<Ret, Args...>(INIT_HASH64);
		IReflectionStaticFunctionBase* const func = getStaticFunc(name, arg_hash);
		
		if (func) {
			return static_cast<IReflectionStaticFunction<Ret, Args...>*>(func);
		}

		return nullptr;
	}

	virtual ~IReflectionDefinition(void) {}

	virtual const IReflection& getReflectionInstance(void) const = 0;
	virtual int32_t size(void) const = 0;

	virtual bool isPolymorphic(void) const = 0;
	virtual bool isBuiltIn(void) const = 0;

	virtual const char* getFriendlyName(void) const = 0;

	virtual bool load(const ISerializeReader& reader, void* object, bool refl_load = false) const = 0;
	virtual void save(ISerializeWriter& writer, const void* object, bool refl_save = false) const = 0;

	virtual Hash64 getInstanceHash(const void* object, Hash64 init = INIT_HASH64) const = 0;

	virtual const void* getInterface(Hash64 class_id, const void* object) const = 0;
	virtual void* getInterface(Hash64 class_id, void* object) const = 0;
	virtual bool hasInterface(Hash64 class_hash) const = 0;

	virtual IAllocator& getAllocator(void) = 0;

	virtual int32_t getNumVars(void) const = 0;
	virtual const char* getVarName(int32_t index) const = 0;
	virtual Hash32 getVarHash(int32_t index) const = 0;
	virtual IReflectionVar* getVar(int32_t index) const = 0;
	virtual IReflectionVar* getVar(Hash32 name) const = 0;

	virtual int32_t getNumFuncs(void) const = 0;
	virtual int32_t getNumFuncOverrides(int32_t index) const = 0;
	virtual const char* getFuncName(int32_t index) const = 0;
	virtual Hash32 getFuncHash(int32_t index) const = 0;
	virtual int32_t getFuncIndex(Hash32 name) const = 0;

	virtual int32_t getNumStaticFuncs(void) const = 0;
	virtual int32_t getNumStaticFuncOverrides(int32_t index) const = 0;
	virtual const char* getStaticFuncName(int32_t) const = 0;
	virtual Hash32 getStaticFuncHash(int32_t index) const = 0;
	virtual int32_t getStaticFuncIndex(Hash32 name) const = 0;

	virtual int32_t getNumClassAttrs(void) const = 0;
	virtual const IAttribute* getClassAttr(Hash64 attr_name) const = 0;
	virtual const IAttribute* getClassAttr(int32_t index) const = 0;

	virtual int32_t getNumVarAttrs(Hash32 name) const = 0;
	virtual const IAttribute* getVarAttr(Hash32 name, Hash64 attr_name) const = 0;
	virtual const IAttribute* getVarAttr(Hash32 name, int32_t index) const = 0;

	virtual int32_t getNumFuncAttrs(Hash32 name) const = 0;
	virtual const IAttribute* getFuncAttr(Hash32 name, Hash64 attr_name) const = 0;
	virtual const IAttribute* getFuncAttr(Hash32 name, int32_t index) const = 0;

	virtual int32_t getNumStaticFuncAttrs(Hash32 name) const = 0;
	virtual const IAttribute* getStaticFuncAttr(Hash32 name, Hash64 attr_name) const = 0;
	virtual const IAttribute* getStaticFuncAttr(Hash32 name, int32_t index) const = 0;

	virtual int32_t getNumConstructors(void) const = 0;
	virtual IReflectionStaticFunctionBase* getConstructor(int32_t index) const = 0;
	virtual VoidFunc getConstructor(Hash64 ctor_hash) const = 0;

	virtual VoidFunc getFactory(Hash64 ctor_hash) const = 0;

	virtual IReflectionStaticFunctionBase* getStaticFunc(int32_t name_index, int32_t override_index) const = 0;
	virtual IReflectionStaticFunctionBase* getStaticFunc(Hash32 name, Hash64 args) const = 0;
	virtual IReflectionFunctionBase* getFunc(int32_t name_index, int32_t override_index) const = 0;
	virtual IReflectionFunctionBase* getFunc(Hash32 name, Hash64 args) const = 0;

	virtual void destroyInstance(void* data) const = 0;

private:
	virtual ptrdiff_t getBasePointerOffset(Hash64 interface_name) const = 0;
	virtual void instantiated(void* object) const = 0;
};

class IEnumReflectionDefinition
{
public:
	virtual ~IEnumReflectionDefinition(void) {}

	virtual const IReflection& getReflectionInstance(void) const = 0;
	virtual int32_t size(void) const = 0;

	virtual Hash64 getInstanceHash(const void* object, Hash64 init = INIT_HASH64) const = 0;

	bool load(const ISerializeReader& reader, void* object, bool) const { return load(reader, object); }
	void save(ISerializeWriter& writer, const void* object, bool) const { save(writer, object); }

	virtual bool load(const ISerializeReader& reader, void* object) const = 0;
	virtual void save(ISerializeWriter& writer, const void* object) const = 0;

	virtual int32_t getNumEntries(void) const = 0;
	virtual const char* getEntryNameFromValue(int32_t value) const = 0;
	virtual const char* getEntryNameFromIndex(int32_t index) const = 0;
	virtual int32_t getEntryValue(int32_t index) const = 0;
	virtual int32_t getEntryValue(const char* name) const = 0;
	virtual int32_t getEntryValue(Hash32 name) const = 0;

	virtual int32_t getNumEnumAttrs(void) const = 0;
	virtual const IAttribute* getEnumAttr(int32_t index) const = 0;
};

template <class Derived, class Base>
const Derived* ReflectionCast(const Base& object)
{
	return object.getReflectionDefinition().template getInterface<Derived>(object.getBasePointer());
}

template <class Derived, class Base>
Derived* ReflectionCast(Base& object)
{
	return object.getReflectionDefinition().template getInterface<Derived>(object.getBasePointer());
}

template <class Derived, class Base>
const Derived* InterfaceCast(const Base& object, Hash64 interface_name)
{
	return reinterpret_cast<Derived*>(object.getReflectionDefinition().getInterface(interface_name, object.getBasePointer()));
}

template <class Derived, class Base>
Derived* InterfaceCast(Base& object, Hash64 interface_name)
{
	return reinterpret_cast<Derived*>(object.getReflectionDefinition().getInterface(interface_name, object.getBasePointer()));
}

NS_END
