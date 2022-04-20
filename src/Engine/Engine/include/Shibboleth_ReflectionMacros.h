/************************************************************************************
Copyright (C) 2022 by Nicholas LaCroix

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

#define CLASS_HASH(class_type) Gaff::FNV1aHash64Const(GAFF_STR_U8(class_type))
#define ARG_HASH(...) Gaff::CalcTemplateHash<__VA_ARGS__>(Gaff::k_init_hash64, eastl::array<const char8_t*, Gaff::GetNumArgs<__VA_ARGS__>()>{ GAFF_FOR_EACH_COMMA(GAFF_STR_U8, __VA_ARGS__) })
#define BASE(type) base<type>(GAFF_STR_U8(type))
#define CTOR(...) ctor<__VA_ARGS__>(ARG_HASH(__VA_ARGS__))
#define GET_INTERFACE(class_type, data) getInterface<class_type>(CLASS_HASH(class_type), data)

#define SHIB_REFLECTION_CLASS_DECLARE(type) \
	public: \
		const Refl::IReflectionDefinition& getReflectionDefinition(void) const override; \
		const void* getBasePointer(void) const override \
		{ \
			return this; \
		} \
		void* getBasePointer(void) override \
		{ \
			return this; \
		} \
	private: \
		friend class Refl::Reflection<type>


#define SHIB_REFLECTION_CLASS_DEFINE(type) \
	const Refl::IReflectionDefinition& type::getReflectionDefinition(void) const \
	{ \
		return Refl::Reflection<type>::GetReflectionDefinition(); \
	}

#define SHIB_REFLECTION_DECLARE_COMMON(type) \
	class Reflection<type> final : public ReflectionBase<type> \
	{ \
	public: \
		static constexpr bool HasReflection = true; \
		static void Init(void); \
		template <class ReflectionBuilder> \
		static void BuildReflection(ReflectionBuilder& builder); \
		static Reflection<type>& GetInstance(void) \
		{ \
			return g_instance; \
		} \
		static const RefDefType<type>& GetReflectionDefinition(void) \
		{ \
			GAFF_ASSERT(g_instance._ref_def); \
			return *g_instance._ref_def; \
		} \
		static /*constexpr*/ const char8_t* GetName(void) \
		{ \
			static const auto name = Hash::ClassHashable<type>::GetName(); \
			return name.data.data(); \
		} \
		static constexpr Gaff::Hash64 GetHash(void) \
		{ \
			return Hash::ClassHashable<type>::GetHash(); \
		} \
		const char8_t* getName(void) const override \
		{ \
			return GetName(); \
		} \
		Gaff::Hash64 getHash(void) const override \
		{ \
			return GetHash(); \
		} \
		void init(void) override \
		{ \
			Init(); \
		} \
	private: \
		static Reflection<type> g_instance; \
	}; \
NS_END

#define SHIB_REFLECTION_DECLARE(type) \
	NS_HASHABLE \
		GAFF_CLASS_HASHABLE(type); \
	NS_END \
	NS_REFLECTION \
	template <> \
	SHIB_REFLECTION_DECLARE_COMMON(type)

#define SHIB_REFLECTION_DEFINE_BEGIN(type) \
	NS_REFLECTION \
		Reflection<type> Reflection<type>::g_instance; \
		void Reflection<type>::Init(void) \
		{ \
			if (g_instance._defined) { \
				return; \
			} \
			GAFF_ASSERT(!g_instance._ref_def); \
			BuildReflection(g_instance._version); \
			const RefDefInterface<type>* ref_def_interface = nullptr; \
			Shibboleth::IApp& app = Shibboleth::GetApp(); \
			if constexpr (std::is_enum<type>::value) { \
				ref_def_interface = reinterpret_cast<const RefDefInterface<type>*>(app.getReflectionManager().getEnumReflection(GetHash())); /* To calm the compiler, even though this should be compiled out ... */ \
			} else { \
				ref_def_interface = reinterpret_cast<const RefDefInterface<type>*>(app.getReflectionManager().getReflection(GetHash())); /* To calm the compiler, even though this should be compiled out ... */ \
			} \
			g_instance._ref_def = static_cast<RefDefType<type>*>( \
				const_cast<RefDefInterface<type>*>(ref_def_interface) \
			); \
			if (g_instance._ref_def) { \
				GAFF_ASSERT_MSG( \
					g_instance._version.getHash() == g_instance._ref_def->getReflectionInstance().getVersion(), \
					"Version hash for %s does not match!", \
					GetName() \
				); \
			} else { \
				Shibboleth::ProxyAllocator allocator("Reflection"); \
				g_instance._ref_def = SHIB_ALLOCT(GAFF_SINGLE_ARG(RefDefType<type>), allocator); \
				g_instance._ref_def->setAllocator(allocator); \
				BuildReflection(*g_instance._ref_def); \
				app.getReflectionManager().registerReflection(g_instance._ref_def); \
			} \
			g_instance._defined = true; \
			for (auto& func : g_instance._on_defined_callbacks) { \
				func(); \
			} \
			g_instance._on_defined_callbacks.clear(); \
		} \
		template <class ReflectionBuilder> \
		void Reflection<type>::BuildReflection(ReflectionBuilder& builder) \
		{ \
			GCC_DISABLE_WARNING_PUSH("-Wunused-value") \
			builder

#define SHIB_REFLECTION_DEFINE_END(type) \
			; \
			builder.finish(); \
			GCC_DISABLE_WARNING_POP() \
		} \
	NS_END

#define SHIB_REFLECTION_DEFINE(type) \
	SHIB_REFLECTION_DEFINE_BEGIN(type) \
	SHIB_REFLECTION_DEFINE_END(type)

#define SHIB_REFLECTION_DEFINE_WITH_CTOR_AND_BASE_NO_INHERITANCE(type, base_class) \
	SHIB_REFLECTION_DEFINE_BEGIN(type) \
		.template BASE(base_class) \
		.template ctor<>() \
	SHIB_REFLECTION_DEFINE_END(type)

#define SHIB_REFLECTION_DEFINE_WITH_BASE_NO_INHERITANCE(type, base_class) \
	SHIB_REFLECTION_DEFINE_BEGIN(type) \
		.template BASE(base_class) \
	SHIB_REFLECTION_DEFINE_END(type)

#define SHIB_REFLECTION_DEFINE_WITH_CTOR_AND_BASE(type, base_class) \
	SHIB_REFLECTION_DEFINE_BEGIN(type) \
		.template base<base_class>() \
		.template ctor<>() \
	SHIB_REFLECTION_DEFINE_END(type)


#define SHIB_TEMPLATE_REFLECTION_CLASS_DECLARE(type, ...) SHIB_REFLECTION_CLASS_DECLARE(type<GAFF_SINGLE_ARG(__VA_ARGS__)>)
#define SHIB_TEMPLATE_REFLECTION_CLASS_DEFINE(type, ...) \
	template < GAFF_FOR_EACH_COMMA(GAFF_PREPEND_CLASS, __VA_ARGS__) > \
	const Refl::IReflectionDefinition& type<__VA_ARGS__>::getReflectionDefinition(void) const \
	{ \
		return Refl::Reflection< type<__VA_ARGS__> >::GetReflectionDefinition(); \
	}

#define SHIB_TEMPLATE_REFLECTION_DECLARE(type, ...) \
	NS_HASHABLE \
		GAFF_TEMPLATE_CLASS_HASHABLE(type, __VA_ARGS__); \
	NS_END \
	NS_REFLECTION \
		template < GAFF_FOR_EACH_COMMA(GAFF_PREPEND_CLASS, __VA_ARGS__) > \
		SHIB_REFLECTION_DECLARE_COMMON(type<GAFF_SINGLE_ARG(__VA_ARGS__)>)

#define SHIB_TEMPLATE_REFLECTION_DEFINE_BEGIN(type, ...) \
	NS_REFLECTION \
		template < GAFF_FOR_EACH_COMMA(GAFF_PREPEND_CLASS, __VA_ARGS__) > \
		Reflection< type<__VA_ARGS__> > Reflection< type<__VA_ARGS__> >::g_instance; \
		template < GAFF_FOR_EACH_COMMA(GAFF_PREPEND_CLASS, __VA_ARGS__) > \
		void Reflection< type<__VA_ARGS__> >::Init(void) \
		{ \
			if (g_instance._defined) { \
				return; \
			} \
			GAFF_ASSERT(!g_instance._ref_def); \
			BuildReflection(g_instance._version); \
			Shibboleth::IApp& app = Shibboleth::GetApp(); \
			const IReflectionDefinition* ref_def_interface = app.getReflectionManager().getReflection(GetHash()); \
			g_instance._ref_def = static_cast<ReflectionDefinition<type<__VA_ARGS__> >*>( \
				const_cast<IReflectionDefinition*>(ref_def_interface) \
			); \
			if (g_instance._ref_def) { \
				GAFF_ASSERT_MSG( \
					g_instance._version.getHash() == g_instance._ref_def->getReflectionInstance().getVersion(), \
					"Version hash for %s does not match!", \
					GetName() \
				); \
			} else { \
				Shibboleth::ProxyAllocator allocator("Reflection"); \
				g_instance._ref_def = SHIB_ALLOCT(GAFF_SINGLE_ARG(ReflectionDefinition< type<__VA_ARGS__> >), allocator); \
				g_instance._ref_def->setAllocator(allocator); \
				BuildReflection(*g_instance._ref_def); \
				app.getReflectionManager().registerReflection(g_instance._ref_def); \
			} \
			g_instance._defined = true; \
			for (auto& func : g_instance._on_defined_callbacks) { \
				func(); \
			} \
			g_instance._on_defined_callbacks.clear(); \
		} \
		template < GAFF_FOR_EACH_COMMA(GAFF_PREPEND_CLASS, __VA_ARGS__) > \
		template <class ReflectionBuilder> \
		void Reflection< type<__VA_ARGS__> >::BuildReflection(ReflectionBuilder& builder) \
		{ \
			builder

#define SHIB_TEMPLATE_REFLECTION_DEFINE_END(type, ...) SHIB_REFLECTION_DEFINE_END(type<GAFF_SINGLE_ARG(__VA_ARGS__)>)
