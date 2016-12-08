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

//#ifdef PLATFORM_WINDOWS
//	#pragma warning(push)
//	#pragma warning(disable : 4996)
//#endif

NS_SHIBBOLETH

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

#define TEMP_DECL_SIZE 1024

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
	// Because VS2015 complains that this function does't reference memory ...
	GAFF_REF(memory);
	reinterpret_cast<T*>(memory)->~T();
}


template <class T>
AngelScriptClassRegister<T>::AngelScriptClassRegister(asIScriptEngine* engine):
	_engine(engine)
{
	const auto& ref_def = Reflection<T>::GetReflectionDefinition();
	int size = 0;

	if (ref_def.hasInterface(REFL_HASH_STRING_CONST("IRefCounted"))) {
		_flags |= asOBJ_REF;
	} else {
		_flags |= asOBJ_VALUE;
		size = static_cast<int>(sizeof(T));
	}

	_engine->RegisterObjectType(Reflection<T>::GetName(), size, _flags);

	if (_flags & asOBJ_REF) {
		// Register factories.
	} else {
		char temp[TEMP_DECL_SIZE] = { 0 };
		snprintf(temp, TEMP_DECL_SIZE, "void %s()", Reflection<T>::GetName());
		_engine->RegisterObjectBehaviour(Reflection<T>::GetName(), asBEHAVE_DESTRUCT, temp, asFUNCTION(ObjectDestructor<T>), asCALL_CDECL_OBJFIRST);
	}
}

template <class T>
template <class Base>
AngelScriptClassRegister<T>& AngelScriptClassRegister<T>::base(const char* /*name*/, Gaff::ReflectionHash /*hash*/)
{
	//r = engine->RegisterObjectMethod("base", "derived@ opCast()", asFUNCTION((refCast<base, derived>)), asCALL_CDECL_OBJLAST); assert(r >= 0);
	//r = engine->RegisterObjectMethod("derived", "base@ opImplCast()", asFUNCTION((refCast<derived, base>)), asCALL_CDECL_OBJLAST); assert(r >= 0);

	// Only register if this type actually has reflection.
	if (Reflection<Base>::HasReflection) {
		AngelScriptClassBaseRegister<T, Base> ascbr(_engine);
		Base::BuildReflection(ascbr);
	}

	return *this;
}

template <class T>
template <class Base>
AngelScriptClassRegister<T>& AngelScriptClassRegister<T>::base(void)
{
	// Register base class offsets
	AngelScriptClassBaseRegister<T, Base> ascbr(_engine);
	Base::BuildReflection(ascbr);
	return *this;
}

template <class T>
template <class... Args>
AngelScriptClassRegister<T>& AngelScriptClassRegister<T>::ctor(void)
{
	if (_flags & asOBJ_REF) {
	} else {
		char temp_a[TEMP_DECL_SIZE] = { 0 };
		char temp_b[TEMP_DECL_SIZE] = { 0 };

		snprintf(temp_a, TEMP_DECL_SIZE, "void %s(", Reflection<T>::GetName());
		const char* decl = GenFuncDecl<Args...>(temp_a, temp_b);

		_engine->RegisterObjectBehaviour(Reflection<T>::GetName(), asBEHAVE_CONSTRUCT, decl, asFUNCTION(GAFF_SINGLE_ARG(ObjectConstructor<T, Args...>)), asCALL_CDECL_OBJFIRST);
	}

	return *this;
}

template <class T>
template <class Var, size_t size>
AngelScriptClassRegister<T>& AngelScriptClassRegister<T>::var(const char (&name)[size], Var T::*ptr)
{
	char temp[TEMP_DECL_SIZE] = { 0 };
	snprintf(temp, TEMP_DECL_SIZE, "%s %s", GetNameString(Reflection<Var>::GetName()), name);

	int offset = static_cast<int>(Gaff::OffsetOfMember(ptr));
	_engine->RegisterObjectProperty(Reflection<T>::GetName(), temp, offset);
	return *this;
}

template <class T>
template <class Ret, class Var, size_t size>
AngelScriptClassRegister<T>& AngelScriptClassRegister<T>::var(const char (&name)[size], Ret (T::*getter)(void) const, void (T::*setter)(Var))
{
	asSFuncPtr func_getter = asSMethodPtr<sizeof(void (T::*)())>::Convert((void (T::*)())(getter));
	asSFuncPtr func_setter = asSMethodPtr<sizeof(void (T::*)())>::Convert((void (T::*)())(setter));

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

	_engine->RegisterObjectMethod(Reflection<T>::GetName(), temp, getter);

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

	_engine->RegisterObjectMethod(Reflection<T>::GetName(), temp, setter);
	return *this;
}

template <class T>
template <size_t size, class Ret, class... Args>
AngelScriptClassRegister<T>& AngelScriptClassRegister<T>::func(const char (&name)[size], Ret (T::*ptr)(Args...) const)
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

	asSFuncPtr func = asSMethodPtr<sizeof(void (T::*)())>::Convert((void (T::*)())(ptr));
	char* decl = GenFuncDecl<Args...>(temp_a, temp_b);

	snprintf((decl == temp_a) ? temp_b : temp_a, TEMP_DECL_SIZE, "%s const", decl);
	decl = (decl == temp_a) ? temp_b : temp_a;

	_engine->RegisterObjectMethod(Reflection<T>::GetName(), decl, func, asCALL_THISCALL);
	return *this;
}

template <class T>
template <size_t size, class Ret, class... Args>
AngelScriptClassRegister<T>& AngelScriptClassRegister<T>::func(const char (&name)[size], Ret (T::*ptr)(Args...))
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

	asSFuncPtr func = asSMethodPtr<sizeof(void (T::*)())>::Convert((void (T::*)())(ptr));
	const char* decl = GenFuncDecl<Args...>(temp_a, temp_b);

	_engine->RegisterObjectMethod(Reflection<T>::GetName(), decl, func, asCALL_THISCALL);
	return *this;
}

template <class T>
void AngelScriptClassRegister<T>::finish(void)
{
}

NS_END

//#ifdef PLATFORM_WINDOWS
//	#pragma warning(pop)
//#endif
