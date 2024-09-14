/************************************************************************************
Copyright (C) 2024 by Nicholas LaCroix

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
#include <Gaff_Ops.h>

NS_REFLECTION

template <class T>
struct IsConstRefHelper final
{
	static constexpr bool k_value = false;
};

template <class T>
struct IsConstRefHelper<const T&> final
{
	static constexpr bool k_value = true;
};

template <class T>
using IsConstRef = IsConstRefHelper<T>::k_value;

template <bool is_const, class T, class Ret, class... Args>
struct ExtensionFuncTypeHelper;

template <class T, class Ret, class... Args>
struct ExtensionFuncTypeHelper<true, T, Ret, Args...> final
{
	using Type = Ret (*)(const T&, Args...);
};

template <class T, class Ret, class... Args>
struct ExtensionFuncTypeHelper<false, T, Ret, Args...> final
{
	using Type = Ret (*)(T&, Args...);
};

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
using ExtensionFuncType = typename ExtensionFuncTypeHelper<is_const, T, Ret, Args...>::Type;

template <bool is_const, class T, class Ret, class... Args>
using MemFuncType = typename MemFuncTypeHelper<is_const, T, Ret, Args...>::Type;

using IAttributePtr = Shibboleth::UniquePtr<IAttribute>;
using AttributeList = Shibboleth::Vector<IAttributePtr>;

template <class FuncPtrType>
struct FuncData final
{
	struct Override final
	{
		AttributeList attrs{ REFLECTION_ALLOCATOR };
		FuncPtrType func;
	};

	Shibboleth::VectorMap<Gaff::Hash64, Override> overrides{ REFLECTION_ALLOCATOR };
};

struct VarData final
{
	AttributeList attrs{ REFLECTION_ALLOCATOR };
	IVarPtr var;
};

struct BaseClassData final
{
	const IReflection* reflection;
	ptrdiff_t offset;
};

struct ConstructData final
{
	IRefStaticFuncPtr ctor;
	VoidFunc factory;
};

template <class T>
struct ReflectionData final
{
	using LoadFunc = bool (*)(const Shibboleth::ISerializeReader&, T&);
	using SaveFunc = void (*)(Shibboleth::ISerializeWriter&, const T&);
	using InstanceHashFunc = Gaff::Hash64 (*)(const T&, Gaff::Hash64);

	using IRefStaticFuncPtr = Shibboleth::UniquePtr<IReflectionStaticFunctionBase>;
	using IRefFuncPtr = Shibboleth::UniquePtr<IReflectionFunctionBase>;
	using IVarPtr = Shibboleth::UniquePtr< IVar<T> >;

	using VoidFunc = void (*)(void);

	Shibboleth::VectorMap<Shibboleth::HashString32<>, VarData> vars{ REFLECTION_ALLOCATOR };
	Shibboleth::VectorMap< Shibboleth::HashString32<>, FuncData<IRefFuncPtr> > funcs{ REFLECTION_ALLOCATOR };
	Shibboleth::VectorMap< Shibboleth::HashString32<>, FuncData<IRefStaticFuncPtr> > static_funcs{ REFLECTION_ALLOCATOR };
	Shibboleth::VectorMap<Gaff::Hash64, ConstructData> factories{ REFLECTION_ALLOCATOR };
	Shibboleth::VectorMap<Gaff::Hash64, BaseClassData> base_classes{ REFLECTION_ALLOCATOR };

	AttributeList class_attrs{ REFLECTION_ALLOCATOR };

	InstanceHashFunc instance_hash = nullptr;
	LoadFunc serialize_load = nullptr;
	SaveFunc serialize_save = nullptr;
};

template <class T, class BaseType>
class ReflectionBuilder final : public IReflectionDefinition
{
public:
	static_assert(std::is_same_v<T, BaseType> || std::is_base_of_v<BaseType, T>);
	static constexpr bool k_is_initial_type = std::is_same_v<T, BaseType>;

	ReflectionBuilder(ReflectionData<T>& data);

	template <class Base>
	ReflectionBuilder& base(const char8_t* name);

	template <class Base>
	ReflectionBuilder& base(void);

	template <class... Args>
	ReflectionBuilder& ctor(Gaff::Hash64 factory_hash);

	template <class... Args>
	ReflectionBuilder& ctor(void);

	template <class Var, size_t name_size, class... Attrs>
	ReflectionBuilder& var(const char8_t (&name)[name_size], Var T::* ptr, const Attrs&... attributes);

	template <class Var, size_t name_size, class... Attrs>
	ReflectionBuilder& var(const char (&name)[name_size], Var T::* ptr, const Attrs&... attributes);

	template <class Ret, class Var, size_t name_size, class... Attrs>
	ReflectionBuilder& var(const char8_t (&name)[name_size], Ret (T::*getter)(void) const, void (T::*setter)(Var), const Attrs&... attributes);

	template <class Ret, class Var, size_t name_size, class... Attrs>
	ReflectionBuilder& var(const char (&name)[name_size], Ret (T::*getter)(void) const, void (T::*setter)(Var), const Attrs&... attributes);

	template <size_t name_size, class Ret, class... Args, class... Attrs>
	ReflectionBuilder& func(const char8_t (&name)[name_size], Ret (T::*ptr)(Args...) const, const Attrs&... attributes);

	template <size_t name_size, class Ret, class... Args, class... Attrs>
	ReflectionBuilder& func(const char (&name)[name_size], Ret (T::*ptr)(Args...) const, const Attrs&... attributes);

	template <size_t name_size, class Ret, class... Args, class... Attrs>
	ReflectionBuilder& func(const char8_t (&name)[name_size], Ret (T::*ptr)(Args...), const Attrs&... attributes);

	template <size_t name_size, class Ret, class... Args, class... Attrs>
	ReflectionBuilder& func(const char (&name)[name_size], Ret (T::*ptr)(Args...), const Attrs&... attributes);

	template <size_t name_size, class Ret, class... Args, class... Attrs>
	ReflectionBuilder& func(const char8_t (&name)[name_size], Ret (*ptr)(const T&, Args...), const Attrs&... attributes);

	template <size_t name_size, class Ret, class... Args, class... Attrs>
	ReflectionBuilder& func(const char (&name)[name_size], Ret (*ptr)(const T&, Args...), const Attrs&... attributes);

	template <size_t name_size, class Ret, class... Args, class... Attrs>
	ReflectionBuilder& func(const char8_t (&name)[name_size], Ret (*ptr)(T&, Args...), const Attrs&... attributes);

	template <size_t name_size, class Ret, class... Args, class... Attrs>
	ReflectionBuilder& func(const char (&name)[name_size], Ret (*ptr)(T&, Args...), const Attrs&... attributes);

	template <size_t name_size, class Ret, class... Args, class... Attrs>
	ReflectionBuilder& staticFunc(const char8_t (&name)[name_size], Ret (*ptr)(Args...), const Attrs&... attributes);

	template <size_t name_size, class Ret, class... Args, class... Attrs>
	ReflectionBuilder& staticFunc(const char (&name)[name_size], Ret (*ptr)(Args...), const Attrs&... attributes);

	template <class Other, class... Attrs>
	ReflectionBuilder& opAdd(const Attrs&... attributes);
	template <class Other, class... Attrs>
	ReflectionBuilder& opSub(const Attrs&... attributes);
	template <class Other, class... Attrs>
	ReflectionBuilder& opMul(const Attrs&... attributes);
	template <class Other, class... Attrs>
	ReflectionBuilder& opDiv(const Attrs&... attributes);
	template <class Other, class... Attrs>
	ReflectionBuilder& opMod(const Attrs&... attributes);

	template <class Other, class... Attrs>
	ReflectionBuilder& opBitAnd(const Attrs&... attributes);
	template <class Other, class... Attrs>
	ReflectionBuilder& opBitOr(const Attrs&... attributes);
	template <class Other, class... Attrs>
	ReflectionBuilder& opBitXor(const Attrs&... attributes);
	template <class Other, class... Attrs>
	ReflectionBuilder& opBitShiftLeft(const Attrs&... attributes);
	template <class Other, class... Attrs>
	ReflectionBuilder& opBitShiftRight(const Attrs&... attributes);

	template <class Other, class... Attrs>
	ReflectionBuilder& opAnd(const Attrs&... attributes);
	template <class Other, class... Attrs>
	ReflectionBuilder& opOr(const Attrs&... attributes);

	template <class Other, class... Attrs>
	ReflectionBuilder& opEqual(const Attrs&... attributes);
	template <class Other, class... Attrs>
	ReflectionBuilder& opLessThan(const Attrs&... attributes);
	template <class Other, class... Attrs>
	ReflectionBuilder& opGreaterThan(const Attrs&... attributes);
	template <class Other, class... Attrs>
	ReflectionBuilder& opLessThanOrEqual(const Attrs&... attributes);
	template <class Other, class... Attrs>
	ReflectionBuilder& opGreaterThanOrEqual(const Attrs&... attributes);

	template <bool is_const, class Ret, class... Args, class... Attrs>
	ReflectionBuilder& opCall(const Attrs&... attributes);

	template <class Ret, class Other, class... Attrs>
	ReflectionBuilder& opIndex(const Attrs&... attributes);

	template <class... Attrs>
	ReflectionBuilder& opBitNot(const Attrs&... attributes);

	template <class... Attrs>
	ReflectionBuilder& opNegate(const Attrs&... attributes);

	template <class... Attrs>
	ReflectionBuilder& opMinus(const Attrs&... attributes);

	template <class... Attrs>
	ReflectionBuilder& opPlus(const Attrs&... attributes);

	template <int32_t (*to_string_func)(const T&, char8_t*, int32_t)>
	ReflectionBuilder& opToString(const Attrs&... attributes);

	template <class Other, class... Attrs>
	ReflectionBuilder& opComparison(const Attrs&... attributes);

	template <class... Attrs>
	ReflectionBuilder& opPreIncrement(const Attrs&... attributes);

	template <class... Attrs>
	ReflectionBuilder& opPostIncrement(const Attrs&... attributes);

	template <class... Attrs>
	ReflectionBuilder& opPreDecrement(const Attrs&... attributes);

	template <class... Attrs>
	ReflectionBuilder& opPostDecrement(const Attrs&... attributes);

	template <class Other, class... Attrs>
	ReflectionBuilder& opAssignment(const Attrs&... attributes);

	template <class Other, class... Attrs>
	ReflectionBuilder& opAddAssignment(const Attrs&... attributes);

	template <class Other, class... Attrs>
	ReflectionBuilder& opSubAssignment(const Attrs&... attributes);

	template <class Other, class... Attrs>
	ReflectionBuilder& opMulAssignment(const Attrs&... attributes);

	template <class Other, class... Attrs>
	ReflectionBuilder& opDivAssignment(const Attrs&... attributes);

	template <class Other, class... Attrs>
	ReflectionBuilder& opModAssignment(const Attrs&... attributes);

	template <class Other, class... Attrs>
	ReflectionBuilder& opBitAndAssignment(const Attrs&... attributes);

	template <class Other, class... Attrs>
	ReflectionBuilder& opBitOrAssignment(const Attrs&... attributes);

	template <class Other, class... Attrs>
	ReflectionBuilder& opBitXorAssignment(const Attrs&... attributes);

	template <class Other, class... Attrs>
	ReflectionBuilder& opBitShiftLeftAssignment(const Attrs&... attributes);

	template <class Other, class... Attrs>
	ReflectionBuilder& opBitShiftRightAssignment(const Attrs&... attributes);


	template <class... Attrs>
	ReflectionBuilder& classAttrs(const Attrs&... attributes);

	ReflectionBuilder& version(uint32_t version);

	ReflectionBuilder& serialize(LoadFunc serialize_load, SaveFunc serialize_save = nullptr);

private:
	template <bool is_const, class Ret, class... Args>
	class ReflectionExtensionFunction final : public IReflectionFunction<Ret, Args...>
	{
	public:
		ReflectionExtensionFunction(ExtensionFuncType<is_const, T, Ret, Args...> func)
		{
			_func = func;
		}

		Ret call(const void* object, Args&&... args) const override
		{
			GAFF_ASSERT_MSG(is_const, "Reflected function is non-const.");
			return call(const_cast<void*>(object), std::forward<Args>(args)...);
		}

		Ret call(void* object, Args&&... args) const override
		{
			return _func(*reinterpret_cast<T*>(object), std::forward<Args>(args)...);
		}

		bool isConst(void) const override { return is_const; }

		const void* getFunctionPointer(void) const override { return &_func; }
		size_t getFunctionPointerSize(void) const override { return sizeof(ExtensionFuncType<is_const, T, Ret, Args...>); }

		bool isExtensionFunction(void) const override { return true; }

	private:
		ExtensionFuncType<is_const, T, Ret, Args...> _func;
	};

	template <bool is_const, class Ret, class... Args>
	class ReflectionFunction final : public IReflectionFunction<Ret, Args...>
	{
	public:
		ReflectionFunction(MemFuncType<is_const, T, Ret, Args...> func)
		{
			_func = func;
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

		const void* getFunctionPointer(void) const override { return &_func; }
		size_t getFunctionPointerSize(void) const override { return sizeof(MemFuncType<is_const, T, Ret, Args...>); }

	private:
		MemFuncType<is_const, T, Ret, Args...> _func;
	};


	ReflectionData<T>& _data;


	// Variables
	template <class Var, class First, class... Rest>
	void addAttributes(IReflectionVar& ref_var, Var T::*var, Shibboleth::Vector<IAttributePtr>& attrs, const First& first, const Rest&... rest);
	template <class Var, class Ret, class First, class... Rest>
	void addAttributes(IReflectionVar& ref_var, Ret (T::*getter)(void) const, void (T::*setter)(Var), Shibboleth::Vector<IAttributePtr>& attrs, const First& first, const Rest&... rest);

	// Functions
	template <class Ret, class... Args, class First, class... Rest>
	void addAttributes(IReflectionFunction<Ret, Args...>& ref_func, Ret (T::*ptr)(Args...) const, Shibboleth::Vector<IAttributePtr>& attrs, const First& first, const Rest&... rest);
	template <class Ret, class... Args, class First, class... Rest>
	void addAttributes(IReflectionFunction<Ret, Args...>& ref_func, Ret (T::*ptr)(Args...), Shibboleth::Vector<IAttributePtr>& attrs, const First& first, const Rest&... rest);

	// Static Functions
	template <class Ret, class... Args, class First, class... Rest>
	void addAttributes(IReflectionStaticFunction<Ret, Args...>& ref_func, Ret (*ptr)(Args...), Shibboleth::Vector<IAttributePtr>& attrs, const First& first, const Rest&... rest);

	// Class
	template <class First, class... Rest>
	void addAttributes(Shibboleth::Vector<IAttributePtr>& attrs, const First& first, const Rest&... rest);
};



template <class T, class... Args>
void ConstructFuncImpl(T* obj, Args&&... args);

template <class T, class... Args>
T* FactoryFuncImpl(Gaff::IAllocator& allocator, Args&&... args);

NS_END

#include "Shibboleth_ReflectionBuilder.inl"
