/************************************************************************************
Copyright (C) 2017 by Nicholas LaCroix

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

#ifdef PLATFORM_WINDOWS
	#pragma warning(push)
	#pragma warning(disable : 4127)
#endif

NS_SHIBBOLETH

#define TEMP_DECL_SIZE 1024

template<class T>
using AddReleaseFunc = void (T::*)(void) const;

template <bool>
struct RefCountedHelper;

template <>
struct RefCountedHelper<true>
{
	template <class T>
	constexpr static AddReleaseFunc<T> GetAddRef(void)
	{
		return &T::addRef;
	}

	template <class T>
	constexpr static AddReleaseFunc<T> GetRelease(void)
	{
		return &T::release;
	}

	template <class T>
	constexpr static void AddRef(T& instance)
	{
		instance.addRef();
	}
};

template <>
struct RefCountedHelper<false>
{
	template <class T>
	constexpr static AddReleaseFunc<T> GetAddRef(void)
	{
		return nullptr;
	}

	template <class T>
	constexpr static AddReleaseFunc<T> GetRelease(void)
	{
		return nullptr;
	}

	template <class T>
	constexpr static void AddRef(T&)
	{
	}
};

template <class T, class... Args>
void* ASFactoryFunc(Args&&... args)
{
	IAllocator* const allocator = GetAllocator();
	int32_t pool_index = allocator->getPoolIndex("AngelScript");

	T* instance = SHIB_ALLOC_CAST(T, pool_index, *allocator);
	Gaff::Construct(instance, std::forward<Args>(args)...);
	RefCountedHelper<std::is_base_of<Gaff::IRefCounted, T>::value>::AddRef(*instance);
	return instance;
}

template <class T, class B>
T* ReflectionCastRefCounted(B* obj)
{
	T* derived = Gaff::ReflectionCast<T>(*obj);

	if (derived) {
		RefCountedHelper<std::is_base_of<Gaff::IRefCounted, T>::value>::AddRef(*derived);
	}

	return derived;
}

template <class T, class B>
T* ReflectionCastNotRefCounted(B* obj)
{
	T* derived = Gaff::ReflectionCast<T>(*obj);
	return derived;
}

template <class T, class B>
B* ImpliedCastRefCounted(T* obj)
{
	B* base = obj;
	RefCountedHelper<std::is_base_of<Gaff::IRefCounted, B>::value>::AddRef(*base);
	return base;
}

template <class T, class B>
B* ImpliedCast(T* obj)
{
	B* base = obj;
	return base;
}

template <bool>
struct BaseClassHelper;

template <>
struct BaseClassHelper<true>
{
	template <class T, class B>
	static void RegisterBaseClass(asIScriptEngine* engine)
	{
		AngelScriptClassRegister<T, B> ascr(engine);
		Reflection<B>::BuildReflection(ascr);
	}

	template <class T, class B>
	static void RegisterCastFunctions(asIScriptEngine* engine)
	{
		char temp[TEMP_DECL_SIZE] = { 0 };

		asDWORD t_flags = engine->GetTypeInfoByName(Reflection<T>::GetName())->GetFlags();
		asDWORD b_flags = engine->GetTypeInfoByName(Reflection<B>::GetName())->GetFlags();

		// Both need class reflection in order for this to work.
		if (Reflection<T>::HasClassReflection && Reflection<B>::HasClassReflection) {
			const T* (*const_refl_cast)(const B*) = nullptr;
			T* (*refl_cast)(B*) = nullptr;

			if (std::is_base_of<Gaff::IRefCounted, T>::value) {
				const_refl_cast = ReflectionCastRefCounted<const T, const B>;
				refl_cast = ReflectionCastRefCounted<T, B>;

			} else if (t_flags & (asOBJ_REF | asOBJ_NOCOUNT)){
				const_refl_cast = ReflectionCastNotRefCounted<const T, const B>;
				refl_cast = ReflectionCastNotRefCounted<T, B>;
			}

			if (const_refl_cast && refl_cast) {
				snprintf(temp, TEMP_DECL_SIZE, "const %s@ opCast() const", Reflection<T>::GetName());
				engine->RegisterObjectMethod(Reflection<B>::GetName(), temp, asFUNCTION(const_refl_cast), asCALL_CDECL_OBJLAST);

				snprintf(temp, TEMP_DECL_SIZE, "%s@ opCast()", Reflection<T>::GetName());
				engine->RegisterObjectMethod(Reflection<B>::GetName(), temp, asFUNCTION(refl_cast), asCALL_CDECL_OBJLAST);
			}
		}

		const B* (*const_impl_cast)(const T*) = nullptr;
		B* (*impl_cast)(T*) = nullptr;

		if (std::is_base_of<Gaff::IRefCounted, B>::value) {
			const_impl_cast = ImpliedCastRefCounted<const T, const B>;
			impl_cast = ImpliedCastRefCounted<T, B>;

		} else if (b_flags & (asOBJ_REF | asOBJ_NOCOUNT)) {
			const_impl_cast = ImpliedCast<const T, const B>;
			impl_cast = ImpliedCast<T, B>;
		}

		if (const_impl_cast && impl_cast) {
			snprintf(temp, TEMP_DECL_SIZE, "const %s@ opImplCast() const", Reflection<B>::GetName());
			engine->RegisterObjectMethod(Reflection<T>::GetName(), temp, asFUNCTION(const_impl_cast), asCALL_CDECL_OBJLAST);

			snprintf(temp, TEMP_DECL_SIZE, "%s@ opImplCast()", Reflection<B>::GetName());
			engine->RegisterObjectMethod(Reflection<T>::GetName(), temp, asFUNCTION(impl_cast), asCALL_CDECL_OBJLAST);
		}
	}
};

template <>
struct BaseClassHelper<false>
{
	template <class T, class B>
	static void RegisterBaseClass(asIScriptEngine*)
	{
	}

	template <class T, class B>
	static void RegisterCastFunctions(asIScriptEngine*)
	{
	}
};


static const Gaff::Hash64 g_name_hashes[] = {
	Gaff::FNV1aHash64StringConst("int8_t"),
	Gaff::FNV1aHash64StringConst("int16_t"),
	Gaff::FNV1aHash64StringConst("int32_t"),
	Gaff::FNV1aHash64StringConst("int64_t"),
	Gaff::FNV1aHash64StringConst("uint8_t"),
	Gaff::FNV1aHash64StringConst("uint16_t"),
	Gaff::FNV1aHash64StringConst("uint32_t"),
	Gaff::FNV1aHash64StringConst("uint64_t")
};

static const char* g_name_strings[] = {
	"int8",
	"int16",
	"int",
	"int64",
	"uint8",
	"uint16",
	"uint",
	"uint64"
};

static const char* GetNameString(const char* name)
{
	Gaff::Hash64 hash = Gaff::FNV1aHash64String(name);

	for (int32_t i = 0; i < ARRAY_SIZE(g_name_hashes); ++i) {
		if (g_name_hashes[i] == hash) {
			return g_name_strings[i];
		}
	}

	return name;
}

template <class...>
struct GenFuncDeclHelper;

template <class First, class... Rest>
struct GenFuncDeclHelper<First, Rest...>
{
	static char* GenDecl(char* buff_a, char* buff_b, bool first)
	{
		using V = std::remove_reference< std::remove_const<First>::type >::type;

		snprintf(
			buff_b,
			TEMP_DECL_SIZE,
			(first) ? "%s%s%s%s%s" : "%s, %s%s%s%s",
			buff_a,
			(std::is_const<First>::value) ? "const " : "",
			GetNameString(Reflection<V>::GetName()),
			(std::is_reference<First>::value) ? "&" : "",
			(std::is_reference<First>::value) ? " in" : ""
		);

		return GenFuncDeclHelper<Rest...>::GenDecl(buff_b, buff_a, false);
	}
};

template <>
struct GenFuncDeclHelper<>
{
	static char* GenDecl(char* buff_a, char* buff_b, bool)
	{
		snprintf(buff_b, TEMP_DECL_SIZE, "%s)", buff_a);
		return buff_b;
	}
};

template <class... T>
char* GenFuncDecl(char* buff_a, char* buff_b)
{
	return GenFuncDeclHelper<T...>::GenDecl(buff_a, buff_b, true);
}

template <class T, bool is_void>
struct GetNameHelper;

template <class T>
struct GetNameHelper<T, true>
{
	static const char* GetName(void)
	{
		return "void";
	}
};

template <class T>
struct GetNameHelper<T, false>
{
	static const char* GetName(void)
	{
		return GetNameString(Reflection<T>::GetName());
	}
};

template <class T, class... Args>
void ObjectConstructor(void* memory, Args&&... args)
{
	Gaff::Construct(reinterpret_cast<T*>(memory), std::forward<Args>(args)...);
}

template <class T>
void ObjectDestructor(void* memory)
{
	// Because VS2015 complains that this function does't reference "memory" ...
	GAFF_REF(memory);
	reinterpret_cast<T*>(memory)->~T();
}

template <class T, class B>
asIScriptEngine* AngelScriptClassRegister<T, B>::g_engine = nullptr;

template <class T, class B>
AngelScriptClassRegister<T, B>::AngelScriptClassRegister(asIScriptEngine* engine, AngelScriptFlags as_flags)
{
	g_engine = engine;

	if (std::is_same<T, B>()) {
		const auto& ref_def = Reflection<T>::GetReflectionDefinition();
		int size = 0;

		if (ref_def.hasInterface(Gaff::FNV1aHash64Const("Gaff::IRefCounted"))) {
			_flags = asOBJ_REF | ((as_flags & AS_NO_REF_COUNT) ? asOBJ_NOCOUNT : 0);

		} else {
			if (as_flags & AS_VALUE_AS_REF) {
				_flags = asOBJ_REF | asOBJ_NOCOUNT;
			} else {
				_flags = asGetTypeTraits<T>() | asOBJ_VALUE;
				size = static_cast<int>(sizeof(T));
			}
		}

		g_engine->RegisterObjectType(Reflection<T>::GetName(), size, _flags);

		// Is a reference that has ref counting.
		if ((_flags & (asOBJ_REF | asOBJ_NOCOUNT)) == asOBJ_REF) {
			AddReleaseFunc<B> add_ref = RefCountedHelper<std::is_base_of<Gaff::IRefCounted, B>::value>::GetAddRef<B>();
			AddReleaseFunc<B> release = RefCountedHelper<std::is_base_of<Gaff::IRefCounted, B>::value>::GetRelease<B>();

			asSFuncPtr func_add_ref = asSMethodPtr<sizeof(void (B::*)())>::Convert((void (B::*)())(add_ref));
			asSFuncPtr func_release = asSMethodPtr<sizeof(void (B::*)())>::Convert((void (B::*)())(release));

			g_engine->RegisterObjectBehaviour(Reflection<T>::GetName(), asBEHAVE_ADDREF, "void f()", func_add_ref, asCALL_THISCALL);
			g_engine->RegisterObjectBehaviour(Reflection<T>::GetName(), asBEHAVE_RELEASE, "void f()", func_release, asCALL_THISCALL);

		} else if (_flags & asOBJ_VALUE) {
			g_engine->RegisterObjectBehaviour(Reflection<T>::GetName(), asBEHAVE_DESTRUCT, "void f()", asFUNCTION(ObjectDestructor<B>), asCALL_CDECL_OBJFIRST);
		}
	}
}

template <class T, class B>
template <class Base>
AngelScriptClassRegister<T, B>& AngelScriptClassRegister<T, B>::base(const char*)
{
	return base<Base>();
}

template <class T, class B>
template <class Base>
AngelScriptClassRegister<T, B>& AngelScriptClassRegister<T, B>::base(void)
{
	// Only register if this type actually has reflection.
	if (Reflection<Base>::IsDefined()) {
		BaseClassHelper<Reflection<Base>::HasReflection>::RegisterBaseClass<T, Base>(g_engine);
		BaseClassHelper<Reflection<Base>::HasClassReflection>::RegisterCastFunctions<T, Base>(g_engine);

	// Register for callback if base class hasn't been defined yet.
	}
	else if (Reflection<Base>::HasReflection) {
		Reflection<Base>::RegisterOnDefinedCallback(&RegisterBaseClass<Base>);
	}

	return *this;
}

template <class T, class B>
template <class... Args>
AngelScriptClassRegister<T, B>& AngelScriptClassRegister<T, B>::ctor(void)
{
	if (std::is_same<T, B>()) {
		char temp_a[TEMP_DECL_SIZE] = { 0 };
		char temp_b[TEMP_DECL_SIZE] = { 0 };

		// Register factory.
		if (_flags & asOBJ_REF) {
			if (!(_flags & asOBJ_NOCOUNT)) {
				snprintf(temp_a, TEMP_DECL_SIZE, "%s@ f(", Reflection<T>::GetName());
				const char* decl = GenFuncDecl<Args...>(temp_a, temp_b);

				g_engine->RegisterObjectBehaviour(Reflection<T>::GetName(), asBEHAVE_FACTORY, decl, asFUNCTION(GAFF_SINGLE_ARG(ASFactoryFunc<T, Args...>)), asCALL_CDECL);
			}

		} else {
			snprintf(temp_a, TEMP_DECL_SIZE, "%s", "void f(");
			const char* decl = GenFuncDecl<Args...>(temp_a, temp_b);

			g_engine->RegisterObjectBehaviour(Reflection<T>::GetName(), asBEHAVE_CONSTRUCT, decl, asFUNCTION(GAFF_SINGLE_ARG(ObjectConstructor<T, Args...>)), asCALL_CDECL_OBJFIRST);
		}
	}

	return *this;
}

template <class T, class B>
template <class Var, size_t size>
AngelScriptClassRegister<T, B>& AngelScriptClassRegister<T, B>::var(const char (&name)[size], Var B::*ptr)
{
	char temp[TEMP_DECL_SIZE] = { 0 };
	snprintf(temp, TEMP_DECL_SIZE, "%s %s", GetNameString(Reflection<Var>::GetName()), name);

	int offset = static_cast<int>(Gaff::OffsetOfMember(ptr) + Gaff::OffsetOfClass<T, B>());
	g_engine->RegisterObjectProperty(Reflection<T>::GetName(), temp, offset);
	return *this;
}

template <class T, class B>
template <class Ret, class Var, size_t size>
AngelScriptClassRegister<T, B>& AngelScriptClassRegister<T, B>::var(const char (&name)[size], Ret (B::*getter)(void) const, void (B::*setter)(Var))
{
	asSFuncPtr func_getter = asSMethodPtr<sizeof(void (B::*)())>::Convert((void (B::*)())(getter));
	asSFuncPtr func_setter = asSMethodPtr<sizeof(void (B::*)())>::Convert((void (B::*)())(setter));

	using R = std::remove_reference< std::remove_const<Ret>::type >::type;
	using V = std::remove_reference< std::remove_const<Var>::type >::type;

	char temp[TEMP_DECL_SIZE] = { 0 };
	sprintf(
		temp,
		TEMP_DECL_SIZE,
		"%s%s%s %s() const",
		(std::is_const<Ret>::value) ? "const " : "",
		GetNameString(Reflection<R>::GetName()),
		(std::is_reference<Ret>::value) ? "&" : "",
		name
	);

	g_engine->RegisterObjectMethod(Reflection<T>::GetName(), temp, getter);

	snprintf(
		temp,
		TEMP_DECL_SIZE,
		"void %s(%s%s%s%s) const",
		name,
		(std::is_const<Var>::value) ? "const " : "",
		GetNameString(Reflection<V>::GetName()),
		(std::is_reference<Var>::value) ? "&" : ""
		(std::is_reference<Var>::value) ? " in" : ""
	);

	g_engine->RegisterObjectMethod(Reflection<T>::GetName(), temp, setter);
	return *this;
}

template <class T, class B>
template <size_t size, class Ret, class... Args>
AngelScriptClassRegister<T, B>& AngelScriptClassRegister<T, B>::func(const char (&name)[size], Ret (B::*ptr)(Args...) const)
{
	char temp_a[TEMP_DECL_SIZE] = { 0 };
	char temp_b[TEMP_DECL_SIZE] = { 0 };

	using R = std::remove_reference< std::remove_const<Ret>::type >::type;

	snprintf(
		temp_a,
		TEMP_DECL_SIZE,
		"%s%s%s %s(",
		(std::is_const<Ret>::value) ? "const " : "",
		GetNameHelper<R, std::is_void<R>::value>::GetName(),
		(std::is_reference<Ret>::value) ? "&" : "",
		name
	);

	asSFuncPtr func = asSMethodPtr<sizeof(void (B::*)())>::Convert((void (B::*)())(ptr));
	char* decl = GenFuncDecl<Args...>(temp_a, temp_b);

	snprintf((decl == temp_a) ? temp_b : temp_a, TEMP_DECL_SIZE, "%s const", decl);
	decl = (decl == temp_a) ? temp_b : temp_a;

	g_engine->RegisterObjectMethod(Reflection<T>::GetName(), decl, func, asCALL_THISCALL);
	return *this;
}

template <class T, class B>
template <size_t size, class Ret, class... Args>
AngelScriptClassRegister<T, B>& AngelScriptClassRegister<T, B>::func(const char (&name)[size], Ret (B::*ptr)(Args...))
{
	char temp_a[1024] = { 0 };
	char temp_b[1024] = { 0 };

	using R = std::remove_reference< std::remove_const<Ret>::type >::type;

	snprintf(
		temp_a,
		TEMP_DECL_SIZE,
		"%s%s%s %s(",
		(std::is_const<Ret>::value) ? "const " : "",
		//(std::is_void<R>::value) ? "void" : Reflection<R>::GetName(),
		GetNameHelper<R, std::is_void<R>::value>::GetName(),
		(std::is_reference<Ret>::value) ? "&" : "",
		name
	);

	asSFuncPtr func = asSMethodPtr<sizeof(void (B::*)())>::Convert((void (B::*)())(ptr));
	const char* decl = GenFuncDecl<Args...>(temp_a, temp_b);

	g_engine->RegisterObjectMethod(Reflection<T>::GetName(), decl, func, asCALL_THISCALL);
	return *this;
}

template <class T, class B>
template <class Base>
void AngelScriptClassRegister<T, B>::RegisterBaseClass(void)
{
	BaseClassHelper<Reflection<Base>::HasReflection>::RegisterBaseClass<T, Base>(g_engine);
	BaseClassHelper<Reflection<Base>::HasClassReflection>::RegisterCastFunctions<T, Base>(g_engine);
}

template <class T, class B>
template <class... Args>
AngelScriptClassRegister<T, B>& AngelScriptClassRegister<T, B>::classAttrs(const Args&...)
{
	return *this;
}

template <class T, class B>
template <size_t size, class... Args>
AngelScriptClassRegister<T, B>& AngelScriptClassRegister<T, B>::varAttrs(const char(&)[size], const Args&...)
{
	return *this;
}

template <class T, class B>
AngelScriptClassRegister<T, B>& AngelScriptClassRegister<T, B>::attrFile(const char*)
{
	return *this;
}

NS_END

#ifdef PLATFORM_WINDOWS
	#pragma warning(pop)
#endif
