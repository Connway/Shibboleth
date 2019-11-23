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

#include "Gaff_SerializeInterfaces.h"
#include "Gaff_EnumReflectionDefinition.h"
#include "Gaff_ReflectionDefinition.h"
#include "Gaff_ReflectionVersion.h"
#include "Gaff_Hashable.h"
#include "Gaff_Utils.h"

#ifndef GAFF_REFLECTION_NAMESPACE
	#define GAFF_REFLECTION_NAMESPACE Gaff
#endif

#ifndef NS_REFLECTION
	#define NS_REFLECTION namespace GAFF_REFLECTION_NAMESPACE {
#endif

#define GAFF_TEMPLATE_GET_NAME(x) GAFF_HASHABLE_NAMESPACE::GetName<x>()
#define GAFF_TEMPLATE_REFLECTION_CLASS(x) class x

#define GAFF_REFLECTION_DECLARE_COMMON(type, allocator) \
	class Reflection<type> final : public Gaff::IReflection \
	{ \
	private: \
		static Gaff::ReflectionVersion<type> g_version; \
		static Reflection<type> g_instance; \
		static bool g_defined; \
		static Gaff::Vector<eastl::function<void (void)>, allocator> g_on_defined_callbacks; \
		template <class RefT, class RefAllocator> \
		friend class Gaff::EnumReflectionDefinition; \
		template <class RefT, class RefAllocator> \
		friend class Gaff::ReflectionDefinition; \
	public: \
		constexpr static bool HasReflection = true; \
		Reflection(void) \
		{ \
			if (std::is_base_of<Gaff::IAttribute, type>::value) { \
				Gaff::AddToAttributeReflectionChain(this); \
			} else { \
				Gaff::AddToReflectionChain(this); \
			} \
		} \
		bool isEnum(void) const override \
		{ \
			return std::is_enum<type>::value; \
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
		const Gaff::IEnumReflectionDefinition& getEnumReflectionDefinition(void) const override \
		{ \
			if constexpr (std::is_enum<type>::value) { \
				return reinterpret_cast<const Gaff::IEnumReflectionDefinition&>(GetReflectionDefinition()); /* To calm the compiler, even though this should be compiled out ... */ \
			} else { \
				return IReflection::getEnumReflectionDefinition(); \
			} \
		} \
		const Gaff::IReflectionDefinition& getReflectionDefinition(void) const override \
		{ \
			if constexpr (std::is_enum<type>::value) { \
				return IReflection::getReflectionDefinition(); \
			} else { \
				return reinterpret_cast<const Gaff::IReflectionDefinition&>(GetReflectionDefinition()); /* To calm the compiler, even though this should be compiled out ... */ \
			} \
		} \
		void init(void) override \
		{ \
			Init(); \
		} \
		constexpr static Gaff::Hash64 GetHash(void) \
		{ \
			return GAFF_HASHABLE_NAMESPACE::GetHash<type>(); \
		} \
		constexpr static const char* GetName(void) \
		{ \
			return GAFF_HASHABLE_NAMESPACE::GetName<type>(); \
		} \
		static Reflection<type>& GetInstance(void) \
		{ \
			return g_instance; \
		} \
		static Gaff::Hash64 GetVersion(void) \
		{ \
			return g_version.getHash(); \
		} \
		constexpr static int32_t Size(void) \
		{ \
			return sizeof(type); \
		} \
		static bool IsDefined(void) \
		{ \
			return g_defined; \
		} \
		static void RegisterOnDefinedCallback(const eastl::function<void (void)>& callback) \
		{ \
			g_on_defined_callbacks.emplace_back(callback); \
		} \
		static void RegisterOnDefinedCallback(eastl::function<void (void)>&& callback) \
		{ \
			g_on_defined_callbacks.emplace_back(std::move(callback)); \
		} \
		template <class ReflectionBuilder> \
		static void BuildReflection(ReflectionBuilder& builder); \

#define GAFF_REFLECTION_DECLARE_BASE(type, allocator) \
		bool load(const Gaff::ISerializeReader& reader, void* object, bool refl_load = false) const override \
		{ \
			GAFF_ASSERT(object); \
			return g_reflection_definition.load(reader, object, refl_load); \
		} \
		void save(Gaff::ISerializeWriter& writer, const void* object, bool refl_save = false) const override \
		{ \
			GAFF_ASSERT(object); \
			g_reflection_definition.save(writer, object, refl_save); \
		} \
		static bool Load(const Gaff::ISerializeReader& reader, type& object, bool refl_load = false) \
		{ \
			return g_reflection_definition.load(reader, object, refl_load); \
		} \
		static void Save(Gaff::ISerializeWriter& writer, const type& object, bool refl_save = false) \
		{ \
			g_reflection_definition.save(writer, object, refl_save); \
		} \
		static const typename Gaff::RefDefType<type, allocator>& GetReflectionDefinition(void) \
		{ \
			return g_reflection_definition; \
		} \
		static void SetAllocator(const allocator& a) \
		{ \
			g_reflection_definition.setAllocator(a); \
			g_on_defined_callbacks.set_allocator(a); \
		} \
		static void Init(void) \
		{ \
			BuildReflection(g_version); \
			BuildReflection(g_reflection_definition); \
		} \
	private: \
		static typename Gaff::RefDefType<type, allocator> g_reflection_definition; \
	};

#define GAFF_REFLECTION_DECLARE(type, allocator) \
	NS_HASHABLE \
		GAFF_CLASS_HASHABLE(type) \
	NS_END \
	NS_REFLECTION \
		template <> \
		GAFF_REFLECTION_DECLARE_COMMON(type, allocator) \
		GAFF_REFLECTION_DECLARE_BASE(type, allocator) \
	NS_END

#define GAFF_ENUM_REFLECTION_DECLARE GAFF_REFLECTION_DECLARE
#define GAFF_ENUM_REFLECTION_DEFINE GAFF_REFLECTION_EXTERNAL_DEFINE
#define GAFF_ENUM_REFLECTION_BEGIN GAFF_REFLECTION_BUILDER_BEGIN
#define GAFF_ENUM_REFLECTION_END GAFF_REFLECTION_BUILDER_END

#define GAFF_REFLECTION_CLASS_DECLARE(type, allocator) \
	public: \
		using ThisType = type; \
		constexpr static Gaff::Hash64 GetReflectionHash(void) \
		{ \
			return Gaff::FNV1aHash64Const(#type); \
		} \
		constexpr static const char* GetReflectionName(void) \
		{ \
			return #type; \
		} \
		virtual const Gaff::IReflectionDefinition& getReflectionDefinition(void) const override \
		{ \
			return GetReflectionDefinition(); \
		} \
		virtual const void* getBasePointer(void) const override \
		{ \
			return this; \
		} \
		virtual void* getBasePointer(void) override \
		{ \
			return this; \
		} \
		static const Gaff::IReflectionDefinition& GetReflectionDefinition(void); \
		template <class ReflectionBuilder> \
		static void BuildReflection(ReflectionBuilder& builder)

#define GAFF_REFLECTION_DEFINE_BASE(type, allocator) \
	GAFF_REFLECTION_NAMESPACE::Reflection<type> GAFF_REFLECTION_NAMESPACE::Reflection<type>::g_instance; \
	Gaff::ReflectionVersion<type> GAFF_REFLECTION_NAMESPACE::Reflection<type>::g_version; \
	bool GAFF_REFLECTION_NAMESPACE::Reflection<type>::g_defined = false; \
	Gaff::Vector<eastl::function<void (void)>, allocator> GAFF_REFLECTION_NAMESPACE::Reflection<type>::g_on_defined_callbacks

#define GAFF_REFLECTION_EXTERNAL_DEFINE(type, allocator) \
	GAFF_REFLECTION_EXTERNAL_DEFINE_BEGIN(type, allocator) \
	GAFF_REFLECTION_EXTERNAL_DEFINE_END(type, allocator)

#define GAFF_REFLECTION_EXTERNAL_DEFINE_BEGIN(type, allocator) \
	typename Gaff::RefDefType<type, allocator> GAFF_REFLECTION_NAMESPACE::Reflection<type>::g_reflection_definition; \
	GAFF_REFLECTION_DEFINE_BASE(type, allocator); \
	void GAFF_REFLECTION_NAMESPACE::Reflection<type>::Init() \
	{ \
		BuildReflection(g_version); \
		BuildReflection(g_reflection_definition); \

#define GAFF_REFLECTION_EXTERNAL_DEFINE_END GAFF_REFLECTION_DEFINE_END

#define GAFF_REFLECTION_DEFINE(type, allocator) \
	GAFF_REFLECTION_DEFINE_BEGIN_NO_BUILD(type, allocator) \
	GAFF_REFLECTION_DEFINE_END(type, allocator)

#define GAFF_REFLECTION_DEFINE_BEGIN(type, allocator) \
	template <class ReflectionBuilder> \
	void GAFF_REFLECTION_NAMESPACE::Reflection<type>::BuildReflection(ReflectionBuilder& builder) \
	{ \
		type::BuildReflection(builder); \
	} \
	GAFF_REFLECTION_EXTERNAL_DEFINE_BEGIN(type, allocator)

#define GAFF_REFLECTION_DEFINE_END(type, ...) }

#define GAFF_REFLECTION_BUILDER_BEGIN(type) \
	template <class ReflectionBuilder> \
	void GAFF_REFLECTION_NAMESPACE::Reflection<type>::BuildReflection(ReflectionBuilder& builder) \
	{ \
		builder

#define GAFF_REFLECTION_BUILDER_BEGIN_NO_BUILD(type) \
	template <class ReflectionBuilder> \
	void GAFF_REFLECTION_NAMESPACE::Reflection<type>::BuildReflection(ReflectionBuilder& builder) \
	{ \

#define GAFF_REFLECTION_BUILDER_END(type) \
		; \
		builder.finish(); \
	}

#define GAFF_REFLECTION_CLASS_DEFINE(type, allocator) \
	GAFF_REFLECTION_CLASS_DEFINE_BEGIN_NO_BUILD(type, allocator) \
	GAFF_REFLECTION_CLASS_DEFINE_END(type, allocator)

#define GAFF_REFLECTION_CLASS_DEFINE_BEGIN(type, allocator) \
	const Gaff::IReflectionDefinition& type::GetReflectionDefinition(void) \
	{ \
		return GAFF_REFLECTION_NAMESPACE::Reflection<type>::GetReflectionDefinition(); \
	} \
	template <class ReflectionBuilder> \
	void type::BuildReflection(ReflectionBuilder& builder) \
	{ \
		builder

#define GAFF_REFLECTION_CLASS_DEFINE_BEGIN_NO_BUILD(type, allocator) \
	const Gaff::IReflectionDefinition& type::GetReflectionDefinition(void) \
	{ \
		return GAFF_REFLECTION_NAMESPACE::Reflection<type>::GetReflectionDefinition(); \
	} \
	template <class ReflectionBuilder> \
	void type::BuildReflection(ReflectionBuilder& builder) \
	{ \

#define GAFF_REFLECTION_CLASS_DEFINE_END(type, ...) GAFF_REFLECTION_BUILDER_END(type)


// Template Reflection
#define GAFF_TEMPLATE_REFLECTION_DECLARE(type, allocator, ...) \
	NS_HASHABLE \
		GAFF_TEMPLATE_CLASS_HASHABLE(type, __VA_ARGS__) \
	NS_END \
	NS_REFLECTION \
		template < GAFF_FOR_EACH_COMMA(GAFF_TEMPLATE_REFLECTION_CLASS, __VA_ARGS__) > \
		GAFF_REFLECTION_DECLARE_COMMON(GAFF_SINGLE_ARG(type<__VA_ARGS__>), allocator, __VA_ARGS__) \
		GAFF_REFLECTION_DECLARE_BASE(GAFF_SINGLE_ARG(type<__VA_ARGS__>), allocator) \
	NS_END

#define GAFF_TEMPLATE_REFLECTION_CLASS_DECLARE(type, allocator, ...) \
	public: \
		using ThisType = type<__VA_ARGS__>; \
		constexpr static Gaff::Hash64 GetReflectionHash(void) \
		{ \
			return Gaff::CalcTemplateHash<__VA_ARGS__>(Gaff::FNV1aHash64Const(#type)); \
		} \
		static const char* GetReflectionName(void) \
		{ \
			static char s_name[128] = { 0 }; \
			if (!s_name[0]) { \
				char format[64] = { 0 }; \
				int32_t index = snprintf(format, 32, "%s<", #type); \
				for (int32_t i = 0; i < Gaff::GetNumArgs<__VA_ARGS__>(); ++i) { \
					if (i) { \
						format[index++] = ','; \
						format[index++] = ' '; \
					} \
					format[index++] = '%'; \
					format[index++] = 's'; \
				} \
				format[index++] = '>'; \
				snprintf(s_name, 128, format, GAFF_FOR_EACH_COMMA(GAFF_TEMPLATE_GET_NAME, __VA_ARGS__)); \
			} \
			return s_name; \
		} \
		const Gaff::IReflectionDefinition& getReflectionDefinition(void) const override \
		{ \
			return GetReflectionDefinition(); \
		} \
		const void* getBasePointer(void) const override \
		{ \
			return this; \
		} \
		void* getBasePointer(void) override \
		{ \
			return this; \
		} \
		static const Gaff::ReflectionDefinition<type<__VA_ARGS__>, allocator>& GetReflectionDefinition(void); \
		template <class ReflectionBuilder> \
		static void BuildReflection(ReflectionBuilder& builder)

#define GAFF_TEMPLATE_REFLECTION_DEFINE_BASE(type, allocator, ...) \
	template < GAFF_FOR_EACH_COMMA(GAFF_TEMPLATE_REFLECTION_CLASS, __VA_ARGS__) > \
	Gaff::ReflectionVersion< type<__VA_ARGS__> > GAFF_REFLECTION_NAMESPACE::Reflection< type<__VA_ARGS__> >::g_version; \
	template < GAFF_FOR_EACH_COMMA(GAFF_TEMPLATE_REFLECTION_CLASS, __VA_ARGS__) > \
	GAFF_REFLECTION_NAMESPACE::Reflection< type<__VA_ARGS__> > GAFF_REFLECTION_NAMESPACE::Reflection< type<__VA_ARGS__> >::g_instance; \
	template < GAFF_FOR_EACH_COMMA(GAFF_TEMPLATE_REFLECTION_CLASS, __VA_ARGS__) > \
	bool GAFF_REFLECTION_NAMESPACE::Reflection< type<__VA_ARGS__> >::g_defined = false; \
	template < GAFF_FOR_EACH_COMMA(GAFF_TEMPLATE_REFLECTION_CLASS, __VA_ARGS__) > \
	Gaff::Vector<eastl::function<void (void)>, allocator> GAFF_REFLECTION_NAMESPACE::Reflection< type<__VA_ARGS__> >::g_on_defined_callbacks

#define GAFF_TEMPLATE_REFLECTION_EXTERNAL_DEFINE(type, allocator, ...) \
	GAFF_TEMPLATE_REFLECTION_EXTERNAL_DEFINE_BEGIN(type, allocator, __VA_ARGS__) \
	GAFF_TEMPLATE_REFLECTION_EXTERNAL_DEFINE_END(type, allocator, __VA_ARGS__)

#define GAFF_TEMPLATE_REFLECTION_EXTERNAL_DEFINE_BEGIN(type, allocator, ...) \
	template < GAFF_FOR_EACH_COMMA(GAFF_TEMPLATE_REFLECTION_CLASS, __VA_ARGS__) > \
	Gaff::ReflectionDefinition< type<__VA_ARGS__>, allocator> GAFF_REFLECTION_NAMESPACE::Reflection< type<__VA_ARGS__> >::g_reflection_definition; \
	GAFF_TEMPLATE_REFLECTION_DEFINE_BASE(type, allocator, __VA_ARGS__); \
	template < GAFF_FOR_EACH_COMMA(GAFF_TEMPLATE_REFLECTION_CLASS, __VA_ARGS__) > \
	void GAFF_REFLECTION_NAMESPACE::Reflection< type<__VA_ARGS__> >::Init() \
	{ \
		BuildReflection(g_version); \
		BuildReflection(g_reflection_definition); \

#define GAFF_TEMPLATE_REFLECTION_EXTERNAL_DEFINE_END GAFF_REFLECTION_DEFINE_END

#define GAFF_TEMPLATE_REFLECTION_DEFINE(type, allocator, ...) \
	GAFF_TEMPLATE_REFLECTION_DEFINE_BEGIN_NO_BUILD(type, allocator, __VA_ARGS__) \
	GAFF_TEMPLATE_REFLECTION_DEFINE_END(type, allocator, __VA_ARGS__) \

#define GAFF_TEMPLATE_REFLECTION_DEFINE_BEGIN(type, allocator, ...) \
	template < GAFF_FOR_EACH_COMMA(GAFF_TEMPLATE_REFLECTION_CLASS, __VA_ARGS__) > \
	template <class ReflectionBuilder> \
	void GAFF_REFLECTION_NAMESPACE::Reflection< type<__VA_ARGS__> >::BuildReflection(ReflectionBuilder& builder) \
	{ \
		type<__VA_ARGS__>::BuildReflection(builder); \
	} \
	GAFF_TEMPLATE_REFLECTION_EXTERNAL_DEFINE_BEGIN(type, allocator, __VA_ARGS__)

#define GAFF_TEMPLATE_REFLECTION_DEFINE_END GAFF_REFLECTION_DEFINE_END 

#define GAFF_TEMPLATE_REFLECTION_BUILDER_BEGIN(type, ...) \
	template < GAFF_FOR_EACH_COMMA(GAFF_TEMPLATE_REFLECTION_CLASS, __VA_ARGS__) > \
	template <class ReflectionBuilder> \
	void GAFF_REFLECTION_NAMESPACE::Reflection< type<__VA_ARGS__> >::BuildReflection(ReflectionBuilder& builder) \
	{ \
		builder

#define GAFF_TEMPLATE_REFLECTION_BUILDER_BEGIN_NO_BUILD(type, ...) \
	template < GAFF_FOR_EACH_COMMA(GAFF_TEMPLATE_REFLECTION_CLASS, __VA_ARGS__) > \
	template <class ReflectionBuilder> \
	void GAFF_REFLECTION_NAMESPACE::Reflection< type<__VA_ARGS__> >::BuildReflection(ReflectionBuilder& builder) \
	{

#define GAFF_TEMPLATE_REFLECTION_BUILDER_END(type, ...) GAFF_REFLECTION_BUILDER_END(type)

#define GAFF_TEMPLATE_REFLECTION_CLASS_DEFINE(type, allocator, ...) \
	GAFF_TEMPLATE_REFLECTION_CLASS_DEFINE_BEGIN_NO_BUILD(type, allocator, __VA_ARGS__) \
	GAFF_TEMPLATE_REFLECTION_CLASS_DEFINE_END(type, allocator, __VA_ARGS__)

#define GAFF_TEMPLATE_REFLECTION_CLASS_DEFINE_BEGIN(type, allocator, ...) \
	template < GAFF_FOR_EACH_COMMA(GAFF_TEMPLATE_REFLECTION_CLASS, __VA_ARGS__) > \
	const Gaff::ReflectionDefinition<type<__VA_ARGS__>, allocator>& type<__VA_ARGS__>::GetReflectionDefinition(void) \
	{ \
		return GAFF_REFLECTION_NAMESPACE::Reflection< type<__VA_ARGS__> >::GetReflectionDefinition(); \
	} \
	template < GAFF_FOR_EACH_COMMA(GAFF_TEMPLATE_REFLECTION_CLASS, __VA_ARGS__) > \
	template <class ReflectionBuilder> \
	void type<__VA_ARGS__>::BuildReflection(ReflectionBuilder& builder) \
	{ \
		builder

#define GAFF_TEMPLATE_REFLECTION_CLASS_DEFINE_BEGIN_NO_BUILD(type, allocator, ...) \
	template < GAFF_FOR_EACH_COMMA(GAFF_TEMPLATE_REFLECTION_CLASS, __VA_ARGS__) > \
	const Gaff::ReflectionDefinition<type<__VA_ARGS__>, allocator>& type<__VA_ARGS__>::GetReflectionDefinition(void) \
	{ \
		return GAFF_REFLECTION_NAMESPACE::Reflection< type<__VA_ARGS__> >::GetReflectionDefinition(); \
	} \
	template < GAFF_FOR_EACH_COMMA(GAFF_TEMPLATE_REFLECTION_CLASS, __VA_ARGS__) > \
	template <class ReflectionBuilder> \
	void type<__VA_ARGS__>::BuildReflection(ReflectionBuilder& builder) \
	{

#define GAFF_TEMPLATE_REFLECTION_CLASS_DEFINE_END GAFF_REFLECTION_CLASS_DEFINE_END


// This isn't used anywhere. Why did I add this?
#define GAFF_REFLECTION_CLASS_STATIC_INIT(type) \
	static void static__reflection__init__func__##type(void); \
	namespace { \
		struct static__reflection__init__func__##type##__helper \
		{ \
		public: \
			static__reflection__init__func__##type##__helper(void) { static__reflection__init__func__##type(); } \
		}; \
	} \
	static const static__reflection__init__func__##type##__helper GAFF_CAT(g__static_reflection_init_global_, __LINE__); \
	void static__reflection__init__func__##type(void) \
	{ \
		Gaff::Reflection<type>::Init(); \
	}

#define REFLECTION_CAST_PTR_NAME(T, name, object) \
	reinterpret_cast<T*>( \
		const_cast<Gaff::IReflectionDefinition&>((object)->getReflectionDefinition()).getInterface( \
			Gaff::FNV1aHash64Const(name), (object)->getBasePointer() \
		) \
	)

#define REFLECTION_CAST_NAME(T, name, object) *REFLECTION_CAST_PTR_NAME(T, name, &object)
#define REFLECTION_CAST_PTR(T, object) REFLECTION_CAST_PTR_NAME(T, #T, object)
#define REFLECTION_CAST(T, object) *REFLECTION_CAST_PTR(T, &object)

NS_GAFF

template <class C>
class IsClassReflected
{
	// This one catches inheritance.
//	template <class T>
//	constexpr static std::true_type testSignature(const void* (T::*)(void) const);
//
//	template <class T>
//	constexpr static decltype(testSignature(&T::getBasePointer)) test(std::nullptr_t);
//
//	template <class T>
//	constexpr static std::false_type test(...);
//
//public:
//	using type = decltype(test<C>(nullptr));
//	constexpr static bool value = type::value;

	// This one does not catch inheritance.
	template<typename U, const void* (U::*)(void) const> struct SFINAE {};
	template<typename U>
	constexpr static char Test(SFINAE<U, &U::getBasePointer>*);
	template<typename U> 
	constexpr static int Test(...);

public:
	constexpr static bool value = sizeof(Test<C>(0)) == sizeof(char);
};

template <class T, class Allocator, bool is_enum>
struct RefDefTypeHelper;

template <class T, class Allocator>
struct RefDefTypeHelper<T, Allocator, true> final
{
	using Type = EnumReflectionDefinition<T, Allocator>;
	using Interface = IEnumReflectionDefinition;
};

template <class T, class Allocator>
struct RefDefTypeHelper<T, Allocator, false> final
{
	using Type = ReflectionDefinition<T, Allocator>;
	using Interface = IReflectionDefinition;
};

template <class T, class Allocator>
using RefDefInterface = typename RefDefTypeHelper<T, Allocator, std::is_enum<T>::value>::Interface;

template <class T, class Allocator>
using RefDefType = typename RefDefTypeHelper<T, Allocator, std::is_enum<T>::value>::Type;


void AddToAttributeReflectionChain(IReflection* reflection);
IReflection* GetAttributeReflectionChainHead(void);

void AddToReflectionChain(IReflection* reflection);
IReflection* GetReflectionChainHead(void);

void AddToEnumReflectionChain(IReflection* reflection);
IReflection* GetEnumReflectionChainHead(void);

void InitAttributeReflection(void);
void InitClassReflection(void);
void InitEnumReflection(void);

NS_END
