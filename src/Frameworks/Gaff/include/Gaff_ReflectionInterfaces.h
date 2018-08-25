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

#include "Gaff_SerializeInterfaces.h"
#include "Gaff_Vector.h"
#include "Gaff_Assert.h"
#include "Gaff_Hash.h"
#include <array>

#define GET_CLASS_ATTR(T) getClassAttr<T>(Gaff::FNV1aHash64Const(#T))
#define GET_VAR_ATTR(T, var_name) getVarAttr<T>(var_name, Gaff::FNV1aHash64Const(#T))
#define GET_FUNC_ATTR(T, func_name) getFuncAttr<T>(func_name, Gaff::FNV1aHash64Const(#T))
#define GET_STATIC_FUNC_ATTR(T, static_func_name) getStaticFuncAttr<T>(static_func_name, Gaff::FNV1aHash64Const(#T))
#define GET_ENUM_ATTR(T) getEnumAttr<T>(Gaff::FNV1aHash64Const(#T))
#define GET_ENUM_VALUE_ATTR(T, value_name) getEnumValueAttr<T>(value_name, Gaff::FNV1aHash64Const(#T))

#define INTERFACE_CAST(interface_type, object) Gaff::InterfaceCast<interface_type>(object, Gaff::FNV1aHash64Const(#interface_type))

#ifdef _MSC_VER
	#pragma warning(push)
	#pragma warning(disable: 4307)
#endif

#ifndef GAFF_REFLECTION_NAMESPACE
	#define GAFF_REFLECTION_NAMESPACE Gaff
#endif

#ifndef NS_REFLECTION
	#define NS_REFLECTION namespace GAFF_REFLECTION_NAMESPACE {
#endif

// These are defined in here instead of Gaff_Reflection.h for compilation purposes.
#define GAFF_POD_SERIALIZABLE(type, read_write_suffix) \
	template <> \
	class Reflection<type> final : public Gaff::IReflection \
	{ \
	public: \
		constexpr static bool HasReflection = true; \
		void load(const Gaff::ISerializeReader& reader, void* object) const override \
		{ \
			GAFF_ASSERT(object); \
			Load(reader, *reinterpret_cast<type*>(object)); \
		} \
		void save(Gaff::ISerializeWriter& writer, const void* object) const override \
		{ \
			GAFF_ASSERT(object); \
			Save(writer, *reinterpret_cast<const type*>(object)); \
		} \
		void init(void) override \
		{ \
		} \
		const char* getName(void) const override \
		{ \
			return GetName(); \
		} \
		Gaff::Hash64 getHash(void) const override \
		{ \
			return GetHash(); \
		} \
		Gaff::Hash64 getVersion(void) const override \
		{ \
			return GetVersion(); \
		} \
		int32_t size(void) const override \
		{ \
			return sizeof(type); \
		} \
		static void Load(const Gaff::ISerializeReader& reader, type& value) \
		{ \
			value = reader.read##read_write_suffix(); \
		} \
		static void Save(Gaff::ISerializeWriter& writer, type value) \
		{ \
			writer.write##read_write_suffix(value); \
		} \
		constexpr static Gaff::Hash64 GetHash(void) \
		{ \
			return Gaff::FNV1aHash64Const(#type); \
		} \
		constexpr static Gaff::Hash64 GetVersion(void) \
		{ \
			return GetHash(); \
		} \
		constexpr static int32_t Size(void) \
		{ \
			return sizeof(type); \
		} \
		constexpr static const char* GetName(void) \
		{ \
			return #type; \
		} \
		static Reflection<type>& GetInstance(void) \
		{ \
			return g_instance; \
		} \
		static bool IsDefined(void) \
		{ \
			return true; \
		} \
	private: \
		static Reflection<type> g_instance; \
	}


NS_GAFF

class IEnumReflectionDefinition;
class IReflectionDefinition;
class IAllocator;

class IReflection
{
public:
	virtual ~IReflection(void) {}

	virtual void init(void) = 0;

	virtual void load(const ISerializeReader& reader, void* object) const = 0;
	virtual void save(ISerializeWriter& writer, const void* object) const = 0;
	virtual const char* getName(void) const = 0;
	virtual Hash64 getHash(void) const = 0;
	virtual Hash64 getVersion(void) const = 0;
	virtual int32_t size(void) const = 0;

	IReflection* attr_next = nullptr;
	IReflection* next = nullptr;
};

NS_END

NS_REFLECTION

template <class T>
class Reflection final : public Gaff::IReflection
{
public:
	constexpr static bool HasReflection = false;
	constexpr static bool HasClassReflection = false;
	Reflection(void)
	{
		GAFF_ASSERT_MSG(false, "Unknown object type.");
	}
	void load(const Gaff::ISerializeReader& /*reader*/, void* /*object*/) const override
	{
		GAFF_ASSERT_MSG(false, "Unknown object type.");
	}
	void save(Gaff::ISerializeWriter& /*writer*/, const void* /*object*/) const override
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
	static bool IsDefined(void)
	{
		return false;
	}
	static void RegisterOnDefinedCallback(const eastl::function<void (void)>&)
	{
	}
	static void RegisterOnDefinedCallback(eastl::function<void (void)>&&)
	{
	}
};

GAFF_POD_SERIALIZABLE(int8_t, Int8);
GAFF_POD_SERIALIZABLE(int16_t, Int16);
GAFF_POD_SERIALIZABLE(int32_t, Int32);
GAFF_POD_SERIALIZABLE(int64_t, Int64);
GAFF_POD_SERIALIZABLE(uint8_t, UInt8);
GAFF_POD_SERIALIZABLE(uint16_t, UInt16);
GAFF_POD_SERIALIZABLE(uint32_t, UInt32);
GAFF_POD_SERIALIZABLE(uint64_t, UInt64);
GAFF_POD_SERIALIZABLE(float, Float);
GAFF_POD_SERIALIZABLE(double, Double);
GAFF_POD_SERIALIZABLE(bool, Bool);

NS_END


NS_GAFF

class IReflectionObject;

template <class T>
const T* ReflectionCast(const IReflectionObject& object);

template <class T>
T* ReflectionCast(IReflectionObject& object);

template <class T>
const T* InterfaceCast(const IReflectionObject& object, Hash64 interface_name);

template <class T>
T* InterfaceCast(IReflectionObject& object, Hash64 interface_name);

template <class... T>
constexpr int32_t GetNumArgs(void)
{
	return sizeof...(T);
}

template <class T>
constexpr const char* GetTypeName(void)
{
	if constexpr (std::is_void<T>::value) {
		return "void";
	} else {
		return GAFF_REFLECTION_NAMESPACE::Reflection<T>::GetName();
	}
}

constexpr const char* GetTypeNameBegin(const char* type_name)
{
	const char* const const_string = "const";
	const char* const_begin = const_string;
	const char* type_begin = type_name;

	while (*type_begin && *const_begin) {
		// Type isn't const.
		if (*type_begin != *const_begin) {
			return type_name;
		}

		++const_begin;
		++type_begin;
	}

	// len(type_name) < len(const_string)
	if (!(*type_begin)) {
		return type_name;
	}

	// We reaached the end of const_string, so the type name begins with "const"/
	// Move pointer over one more to move over the space.
	return type_begin + 1;
}

constexpr const char* GetTypeNameEnd(const char* type_name)
{
	// Find end of string.
	while (*type_name) {
		++type_name;
	}

	--type_name;

	// Walk back until we hit the actual class name.
	while (*type_name == '*' || *type_name == '&' || *type_name == ' ') {
		--type_name;
	}

	// Move forward one so that length calculation is correct.
	return type_name + 1;
}

template <class T>
constexpr Hash64 CalcTypeHash(Hash64 init, const char* type_string, size_t size)
{
	using NoPtr = typename std::remove_pointer<T>::type;
	using NoRef = typename std::remove_reference<NoPtr>::type;
	//using V = typename std::remove_const<NoRef>::type;

	if constexpr (std::is_const<NoRef>::value) {
		init = FNV1aHash64Const("const", init);
	}

	init = FNV1aHash64Const(type_string, size, init);

	if constexpr (std::is_reference<NoPtr>::value) {
		init = FNV1aHash64Const("&", init);
	}

	if constexpr (std::is_pointer<T>::value) {
		init = FNV1aHash64Const("*", init);
	}

	return init;
}

template <class T>
constexpr Hash64 CalcTypeHash(Hash64 init, const char* type_string)
{
	using NoPtr = typename std::remove_pointer<T>::type;
	using NoRef = typename std::remove_reference<NoPtr>::type;
	//using V = typename std::remove_const<NoRef>::type;

	if constexpr (std::is_const<NoRef>::value) {
		init = FNV1aHash64Const("const", init);
	}

	init = FNV1aHash64StringConst(type_string, init);

	if constexpr (std::is_reference<NoPtr>::value) {
		init = FNV1aHash64Const("&", init);
	}

	if constexpr (std::is_pointer<T>::value) {
		init = FNV1aHash64Const("*", init);
	}

	return init;
}

template <class First, class... Rest>
constexpr Hash64 CalcTemplateHashHelper(Hash64 init, const char** type_names, int32_t index)
{
	const char* const begin = GetTypeNameBegin(*(type_names + index));
	const char* const end = GetTypeNameEnd(*(type_names + index));

	if constexpr (sizeof...(Rest) == 0) {
		return CalcTypeHash<First>(init, begin, static_cast<size_t>(end - begin));
	} else {
		return CalcTemplateHashHelper<Rest...>(CalcTypeHash<First>(init, begin, static_cast<size_t>(end - begin)), type_names, index + 1);
	}
}

template <class First, class... Rest>
constexpr Hash64 CalcTemplateHashHelper(Hash64 init)
{
	using NoPtr = typename std::remove_pointer<First>::type;
	using NoRef = typename std::remove_reference<NoPtr>::type;
	using V = typename std::remove_const<NoRef>::type;

	if constexpr (sizeof...(Rest) == 0) {
		return CalcTypeHash<First>(init, GetTypeName<V>());
	} else {
		return CalcTemplateHashHelper<Rest...>(CalcTypeHash<First>(init, GetTypeName<V>()));
	}
}

template <class... T>
constexpr Hash64 CalcTemplateHash(Hash64 init, std::array<const char*, GetNumArgs<T...>()> type_names)
{
	static_assert(sizeof...(T) == type_names.size(), "Initializer list size must match number of template arguments.");
	static_assert(sizeof...(T) > 0, "Initializer list version of CalcTemplateHash must be non-void.");

	return CalcTemplateHashHelper<T...>(init, type_names.data(), 0);
}

template <class... T>
constexpr Hash64 CalcTemplateHash(Hash64 init)
{
	if constexpr (sizeof...(T) == 0) {
		return FNV1aHash64Const("void", init);
	} else {
		return CalcTemplateHashHelper<T...>(init);
	}
}

enum ReflectionValueType
{
	VT_BOOL = 0,
	VT_INT8,
	VT_INT16,
	VT_INT32,
	VT_INT64,
	VT_UINT8,
	VT_UINT16,
	VT_UINT32,
	VT_UINT64,
	VT_ENUM,
	VT_FLOAT,
	VT_DOUBLE,
	VT_STRING,
	VT_OBJECT,
	VT_SIZE
};

template <class T>
constexpr ReflectionValueType GetRVT(void)
{
	return (std::is_enum<T>()) ?
			VT_ENUM :
			(std::is_class<T>()) ?
				VT_OBJECT :
				VT_SIZE;
}

#define RVT_FUNC(type, value) \
	template <> \
	inline ReflectionValueType GetRVT<type>(void) \
	{ \
		return value; \
	}

RVT_FUNC(bool, VT_BOOL)
RVT_FUNC(int8_t, VT_INT8)
RVT_FUNC(int16_t, VT_INT16)
RVT_FUNC(int32_t, VT_INT32)
RVT_FUNC(int64_t, VT_INT64)
RVT_FUNC(uint8_t, VT_UINT8)
RVT_FUNC(uint16_t, VT_UINT16)
RVT_FUNC(uint32_t, VT_UINT32)
RVT_FUNC(uint64_t, VT_UINT64)
RVT_FUNC(float, VT_FLOAT)
RVT_FUNC(double, VT_DOUBLE)

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

	void setReadOnly(bool read_only) { _read_only = read_only; }
	bool isReadOnly(void) const { return _read_only; }

private:
	bool _read_only = false;
};

class IAttribute : public IReflectionObject
{
public:
	virtual IAttribute* clone(void) const = 0;

	virtual void finish(const Gaff::IReflectionDefinition* /*ref_def*/) {}
	virtual void finish(const Gaff::IEnumReflectionDefinition* /*ref_def*/) {}

	virtual void instantiated(const Gaff::IReflectionDefinition* /*ref_def*/, void* /*object*/) {}

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
	void apply(IReflectionVar* /*ref_var*/, Var T::* /*var*/) {}
	template <class T, class Var, class Ret>
	void apply(IReflectionVar* /*ref_var*/, Ret (T::* /*getter*/)(void) const, void (T::* /*setter*/)(Var)) {}
	template <class T, class Var, class Vec_Allocator, size_t size>
	void apply(IReflectionVar* /*ref_var*/, Vector<Var, Vec_Allocator> T::* /*vec*/) {}
	template <class T, class Var, size_t size>
	void apply(IReflectionVar* /*ref_var*/, Var (T::* /*arr*/)[size]) {}
};

template <class Ret, class... Args>
class IReflectionFunction
{
public:
	virtual ~IReflectionFunction(void) {}

	virtual Ret call(const void* obj, Args... args) const = 0;
	virtual Ret call(void* obj, Args... args) const = 0;
	virtual bool isConst(void) const = 0;
};

#define CREATET(Class, allocator, ...) createT<Class>(Gaff::FNV1aHash64Const(#Class), allocator, __VA_ARGS__)

class IReflectionDefinition
{
public:
	template <class... Args>
	using FactoryFunc = void* (*)(IAllocator&, Args...);

	using VoidFunc = void (*)(void);

	template <class T>
	const void* getBasePointer(const T* object, Hash64 interface_name) const
	{
		const ptrdiff_t offset = getBasePointerOffset(interface_name);
		return reinterpret_cast<const char*>(object) - offset;
	}

	template <class T>
	void* getBasePointer(T* object, Hash64 interface_name) const
	{
		const ptrdiff_t offset = getBasePointerOffset(interface_name);
		return reinterpret_cast<char*>(object) - offset;
	}

	template <class T>
	const void* getBasePointer(const T* object) const
	{
		return getBasePointer(object, GAFF_REFLECTION_NAMESPACE::Reflection<T>::GetHash());
	}

	template <class T>
	void* getBasePointer(T* object) const
	{
		return getBasePointer(object, GAFF_REFLECTION_NAMESPACE::Reflection<T>::GetHash());
	}

	template <class T>
	const T* getInterface(const void* object) const
	{
		return reinterpret_cast<const T*>(getInterface(T::GetReflectionHash(), object));
	}

	template <class T>
	T* getInterface(void* object) const
	{
		return reinterpret_cast<T*>(getInterface(T::GetReflectionHash(), object));
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
		const int32_t size = getNumClassAttrs();

		for (int32_t i = 0; i < size; ++i) {
			const IAttribute* const attribute = getClassAttr(i);
			const void* attr = attribute->getReflectionDefinition().getInterface(
				attr_name, attribute->getBasePointer()
			);

			if (attr) {
				return reinterpret_cast<const T*>(attr);
			}
		}

		return nullptr;
	}

	template <class T>
	const T* getVarAttr(Hash32 var_name, Hash64 attr_name) const
	{
		const int32_t size = getNumVarAttrs(var_name);

		for (int32_t i = 0; i < size; ++i) {
			const IAttribute* const attribute = getVarAttr(var_name, i);
			const void* attr = attribute->getReflectionDefinition().getInterface(
				attr_name, attribute->getBasePointer()
			);

			if (attr) {
				return reinterpret_cast<const T*>(attr);
			}
		}

		return nullptr;
	}

	template <class T>
	const T* getFuncAttr(Hash32 func_name, Hash64 attr_name) const
	{
		const int32_t size = getNumFuncAttrs(func_name);

		for (int32_t i = 0; i < size; ++i) {
			const IAttribute* const attribute = getFuncAttr(func_name, i);
			const void* attr = attribute->getReflectionDefinition().getInterface(
				attr_name, attribute->getBasePointer()
			);

			if (attr) {
				return reinterpret_cast<const T*>(attr);
			}
		}

		return nullptr;
	}

	template <class T>
	const T* getStaticFuncAttr(Hash32 static_func_name, Hash64 attr_name) const
	{
		const int32_t size = getNumStaticFuncAttrs(static_func_name);

		for (int32_t i = 0; i < size; ++i) {
			const IAttribute* const attribute = getStaticFuncAttr(static_func_name, i);
			const void* attr = attribute->getReflectionDefinition().getInterface(
				attr_name, attribute->getBasePointer()
			);

			if (attr) {
				return reinterpret_cast<const T*>(attr);
			}
		}

		return nullptr;
	}

	template <class T>
	const T* getClassAttr(void) const
	{
		const int32_t size = getNumClassAttrs();

		for (int32_t i = 0; i < size; ++i) {
			const IAttribute* const attribute = getClassAttr(i);
			const T* attr = ReflectionCast<T>(*attribute);

			if (attr) {
				return attr;
			}
		}

		return nullptr;
	}

	template <class T>
	const T* getVarAttr(Hash32 name) const
	{
		const int32_t size = getNumVarAttrs(name);

		for (int32_t i = 0; i < size; ++i) {
			const IAttribute* const attribute = getVarAttr(name, i);
			const T* attr = ReflectionCast<T>(*attribute);

			if (attr) {
				return attr;
			}
		}

		return nullptr;
	}

	template <class T>
	const T* getFuncAttr(Hash32 name) const
	{
		const int32_t size = getNumFuncAttrs(name);

		for (int32_t i = 0; i < size; ++i) {
			const IAttribute* const attribute = getFuncAttr(name, i);
			const T* attr = ReflectionCast<T>(*attribute);

			if (attr) {
				return attr;
			}
		}

		return nullptr;
	}

	template <class T>
	const T* getStaticFuncAttr(Hash32 name) const
	{
		const int32_t size = getNumStaticFuncAttrs(name);

		for (int32_t i = 0; i < size; ++i) {
			const IAttribute* const attribute = getStaticFuncAttr(name, i);
			const T* attr = ReflectionCast<T>(*attribute);

			if (attr) {
				return attr;
			}
		}

		return nullptr;
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
	FactoryFunc<Args...> getFactory(void) const
	{
		constexpr Hash64 ctor_hash = Gaff::CalcTemplateHash<Args...>(INIT_HASH64);
		return reinterpret_cast< FactoryFunc<Args...> >(getFactory(ctor_hash));
	}

	template <class Ret, class... Args>
	IReflectionFunction<Ret, Args...>* getFunc(Hash32 name) const
	{
		constexpr Hash64 arg_hash = Gaff::CalcTemplateHash<Ret, Args...>(INIT_HASH64);
		void* functor = getFunc(name, arg_hash);

		if (functor) {
			return reinterpret_cast< IReflectionFunction<Ret, Args...>* >(functor);
		}

		return nullptr;
	}

	virtual ~IReflectionDefinition(void) {}

	virtual const IReflection& getReflectionInstance(void) const = 0;

	virtual void load(const ISerializeReader& reader, void* object) const = 0;
	virtual void save(ISerializeWriter& writer, const void* object) const = 0;

	virtual const void* getInterface(Hash64 class_id, const void* object) const = 0;
	virtual void* getInterface(Hash64 class_id, void* object) const = 0;
	virtual bool hasInterface(Hash64 class_hash) const = 0;

	virtual int32_t getNumVars(void) const = 0;
	virtual Hash32 getVarHash(int32_t index) const = 0;
	virtual IReflectionVar* getVar(int32_t index) const = 0;
	virtual IReflectionVar* getVar(Hash32 name) const = 0;

	virtual int32_t getNumFuncs(void) const = 0;
	virtual Hash32 getFuncHash(int32_t index) const = 0;

	virtual int32_t getNumStaticFuncs(void) const = 0;
	virtual Hash32 getStaticFuncHash(int32_t index) const = 0;

	virtual int32_t getNumClassAttrs(void) const = 0;
	virtual const IAttribute* getClassAttr(int32_t index) const = 0;

	virtual int32_t getNumVarAttrs(Hash32 name) const = 0;
	virtual const IAttribute* getVarAttr(Hash32 name, int32_t index) const = 0;

	virtual int32_t getNumFuncAttrs(Hash32 name) const = 0;
	virtual const IAttribute* getFuncAttr(Hash32 name, int32_t index) const = 0;

	virtual int32_t getNumStaticFuncAttrs(Hash32 name) const = 0;
	virtual const IAttribute* getStaticFuncAttr(Hash32 name, int32_t index) const = 0;

	virtual VoidFunc getFactory(Hash64 ctor_hash) const = 0;
	virtual VoidFunc getStaticFunc(Hash32 name, Hash64 args) const = 0;
	virtual void* getFunc(Hash32 name, Hash64 args) const = 0;

private:
	virtual ptrdiff_t getBasePointerOffset(Hash64 interface_name) const = 0;
	virtual void instantiated(void* object) const = 0;
};

class IEnumReflection
{
public:
	virtual ~IEnumReflection(void) {}

	virtual void init(void) = 0;

	virtual void load(ISerializeReader& reader, void* object) const = 0;
	virtual void save(ISerializeWriter& writer, const void* object) const = 0;
	virtual const char* getName(void) const = 0;;
	virtual Hash64 getHash(void) const = 0;
	virtual Hash64 getVersion(void) const = 0;

	IEnumReflection* next = nullptr;
};

class IEnumReflectionDefinition
{
public:
	template <class T>
	const T* getEnumAttr(Hash64 attr_name) const
	{
		for (int32_t i = 0; i < getNumEnumAttrs(); ++i) {
			const IAttribute* const attribute = getEnumAttr(i);
			const void* attr = attribute->getReflectionDefinition().getInterface(attr_name, attribute->getBasePointer());

			if (attr) {
				return reinterpret_cast<const T*>(attr);
			}
		}

		return nullptr;
	}

	template <class T>
	const T* getEnumAttr(void) const
	{
		for (int32_t i = 0; i < getNumEnumAttrs(); ++i) {
			const IAttribute* const attribute = getEnumAttr(i);
			const T* attr = ReflectionCast<T>(*attribute);

			if (attr) {
				return attr;
			}
		}

		return nullptr;
	}

	template <class T>
	const T* getEntryAttr(Hash32 entry_name) const
	{
		const int32_t size = getNumEntryAttrs(entry_name);

		for (int32_t i = 0; i < size; ++i) {
			const IAttribute* const attribute = getEntryAttr(entry_name, i);
			const T* attr = ReflectionCast<T>(*attribute);

			if (attr) {
				return attr;
			}
		}

		return nullptr;
	}

	virtual ~IEnumReflectionDefinition(void) {}

	virtual const IEnumReflection& getReflectionInstance(void) const = 0;

	virtual void load(ISerializeReader& reader, void* object) const = 0;
	virtual void save(ISerializeWriter& writer, const void* object) const = 0;

	virtual int32_t getNumEntries(void) const = 0;
	virtual const char* getEntryNameFromValue(int32_t value) const = 0;
	virtual const char* getEntryNameFromIndex(int32_t index) const = 0;
	virtual int32_t getEntryValue(int32_t index) const = 0;
	virtual int32_t getEntryValue(const char* name) const = 0;
	virtual int32_t getEntryValue(Hash32 name) const = 0;

	virtual int32_t getNumEnumAttrs(void) const = 0;
	virtual const IAttribute* getEnumAttr(int32_t index) const = 0;

	virtual int32_t getNumEntryAttrs(Hash32 name) const = 0;
	virtual const IAttribute* getEntryAttr(Hash32 name, int32_t index) const = 0;
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

#ifdef _MSC_VER
	#pragma warning(pop)
#endif
