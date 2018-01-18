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
#include "Gaff_ReflectionDefinition.h"
#include "Gaff_ReflectionVersion.h"

#ifndef GAFF_REFLECTION_NAMESPACE
	#define GAFF_REFLECTION_NAMESPACE Gaff
#endif

#define GAFF_TEMPLATE_GET_NAME(x) GAFF_REFLECTION_NAMESPACE::Reflection<x>::GetName()
#define GAFF_TEMPLATE_REFLECTION_CLASS(x) class x

#define GAFF_REFLECTION_DECLARE_SERIALIZE(type, allocator) \
namespace GAFF_REFLECTION_NAMESPACE { \
	template <> \
	class Reflection<type> final : public Gaff::IReflection \
	{ \
	public: \
		constexpr static bool HasReflection = true; \
		constexpr static bool HasClassReflection = false; \
		Reflection(void) \
		{ \
			Gaff::AddToReflectionChain(this); \
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
		void init(void) override \
		{ \
			Init(); \
		} \
		static Reflection<type>& GetInstance(void) \
		{ \
			return g_instance; \
		} \
		constexpr static const char* GetName(void) \
		{ \
			return #type; \
		} \
		constexpr static Gaff::Hash64 GetHash(void) \
		{ \
			return Gaff::FNV1aHash64Const(#type); \
		} \
		static Gaff::Hash64 GetVersion(void) \
		{ \
			return g_version.getHash(); \
		} \
		static const Gaff::ReflectionDefinition<type, allocator>& GetReflectionDefinition(void) \
		{ \
			return g_reflection_definition; \
		} \
		static void Load(const Gaff::ISerializeReader& reader, type& object) \
		{ \
			g_instance.load(reader, &object); \
		} \
		static void Save(Gaff::ISerializeWriter& writer, const type& object) \
		{ \
			g_instance.save(writer, &object); \
		} \
		static void Init(void); \
		void load(const Gaff::ISerializeReader& reader, void* object) const override; \
		void save(Gaff::ISerializeWriter& writer, const void* object) const override; \
	private: \
		static Gaff::ReflectionDefinition<type, allocator> g_reflection_definition; \
		static Gaff::ReflectionVersion<type> g_version; \
		static Reflection<type> g_instance; \
	}; \
}

#define GAFF_REFLECTION_DEFINE_SERIALIZE_INIT(type, serialize_version, allocator) \
	Gaff::ReflectionDefinition<type, allocator> GAFF_REFLECTION_NAMESPACE::Reflection<type>::g_reflection_definition; \
	Gaff::ReflectionVersion<type> GAFF_REFLECTION_NAMESPACE::Reflection<type>::g_version = Gaff::ReflectionVersion<type>(serialize_version); \
	GAFF_REFLECTION_NAMESPACE::Reflection<type> GAFF_REFLECTION_NAMESPACE::Reflection<type>::g_instance; \
	void GAFF_REFLECTION_NAMESPACE::Reflection<type>::Init(void)

#define GAFF_REFLECTION_DEFINE_SERIALIZE_LOAD(type) \
	void GAFF_REFLECTION_NAMESPACE::Reflection<type>::load(const Gaff::ISerializeReader& reader, void* object) const

#define GAFF_REFLECTION_DEFINE_SERIALIZE_SAVE(type) \
	void GAFF_REFLECTION_NAMESPACE::Reflection<type>::save(Gaff::ISerializeWriter& writer, const void* object) const


#define GAFF_REFLECTION_DECLARE_COMMON(type, allocator) \
	class Reflection<type> final : public Gaff::IReflection \
	{ \
	private: \
		static Gaff::ReflectionVersion<type> g_version; \
		static Reflection<type> g_instance; \
		static bool g_defined; \
		static Gaff::Vector<eastl::function<void (void)>, allocator> g_on_defined_callbacks; \
		template <class RefT, class RefAllocator> \
		friend class Gaff::ReflectionDefinition; \
	public: \
		constexpr static bool HasReflection = true; \
		constexpr static bool HasClassReflection = Gaff::IsClassReflected<type>::value; \
		Reflection(void) \
		{ \
			if (std::is_base_of<Gaff::IAttribute, type>::value) { \
				Gaff::AddToAttributeReflectionChain(this); \
			} else { \
				Gaff::AddToReflectionChain(this); \
			} \
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
		void init(void) override \
		{ \
			Init(); \
		} \
		static Reflection<type>& GetInstance(void) \
		{ \
			return g_instance; \
		} \
		static Gaff::Hash64 GetVersion(void) \
		{ \
			return g_version.getHash(); \
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
		void load(const Gaff::ISerializeReader& reader, void* object) const override \
		{ \
			GAFF_ASSERT(object); \
			g_reflection_definition.load(reader, object); \
		} \
		void save(Gaff::ISerializeWriter& writer, const void* object) const override \
		{ \
			GAFF_ASSERT(object); \
			g_reflection_definition.save(writer, object); \
		} \
		static void Load(const Gaff::ISerializeReader& reader, type& object) \
		{ \
			g_reflection_definition.load(reader, object); \
		} \
		static void Save(Gaff::ISerializeWriter& writer, const type& object) \
		{ \
			g_reflection_definition.save(writer, object); \
		} \
		static const Gaff::ReflectionDefinition<type, allocator>& GetReflectionDefinition(void) \
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
		static Gaff::ReflectionDefinition<type, allocator> g_reflection_definition; \
	};

#define GAFF_REFLECTION_DECLARE(type, allocator) \
namespace GAFF_REFLECTION_NAMESPACE { \
	template <> \
	GAFF_REFLECTION_DECLARE_COMMON(type, allocator) \
	constexpr static Gaff::Hash64 GetHash(void) \
	{ \
		return Gaff::FNV1aHash64Const(#type); \
	} \
	constexpr static const char* GetName(void) \
	{ \
		return #type; \
	} \
	GAFF_REFLECTION_DECLARE_BASE(type, allocator) \
NS_END

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
		virtual const Gaff::IReflectionDefinition& getReflectionDefinition(void) const \
		{ \
			return GetReflectionDefinition(); \
		} \
		virtual const void* getBasePointer(void) const \
		{ \
			return this; \
		} \
		virtual void* getBasePointer(void) \
		{ \
			return this; \
		} \
		static const Gaff::ReflectionDefinition<type, allocator>& GetReflectionDefinition(void); \
		template <class ReflectionBuilder> \
		static void BuildReflection(ReflectionBuilder& builder)

#define GAFF_REFLECTION_DEFINE_BASE(type, start_version, allocator) \
	GAFF_REFLECTION_NAMESPACE::Reflection<type> GAFF_REFLECTION_NAMESPACE::Reflection<type>::g_instance; \
	Gaff::ReflectionVersion<type> GAFF_REFLECTION_NAMESPACE::Reflection<type>::g_version = Gaff::ReflectionVersion<type>(start_version); \
	bool GAFF_REFLECTION_NAMESPACE::Reflection<type>::g_defined = false; \
	Gaff::Vector<eastl::function<void (void)>, allocator> GAFF_REFLECTION_NAMESPACE::Reflection<type>::g_on_defined_callbacks

#define GAFF_REFLECTION_EXTERNAL_DEFINE(type, start_version, allocator) \
	GAFF_REFLECTION_EXTERNAL_DEFINE_BEGIN(type, start_version, allocator) \
	GAFF_REFLECTION_EXTERNAL_DEFINE_END(type, allocator)

#define GAFF_REFLECTION_EXTERNAL_DEFINE_BEGIN(type, start_version, allocator) \
	Gaff::ReflectionDefinition<type, allocator> GAFF_REFLECTION_NAMESPACE::Reflection<type>::g_reflection_definition; \
	GAFF_REFLECTION_DEFINE_BASE(type, start_version, allocator); \
	void GAFF_REFLECTION_NAMESPACE::Reflection<type>::Init() \
	{ \
		BuildReflection(g_version); \
		BuildReflection(g_reflection_definition); \

#define GAFF_REFLECTION_EXTERNAL_DEFINE_END GAFF_REFLECTION_DEFINE_END

#define GAFF_REFLECTION_DEFINE(type, allocator) \
	GAFF_REFLECTION_DEFINE_BEGIN(type, allocator) \
	GAFF_REFLECTION_DEFINE_END(type, allocator)

#define GAFF_REFLECTION_DEFINE_BEGIN(type, start_version, allocator) \
	template <class ReflectionBuilder> \
	void GAFF_REFLECTION_NAMESPACE::Reflection<type>::BuildReflection(ReflectionBuilder& builder) \
	{ \
		type::BuildReflection(builder); \
	} \
	GAFF_REFLECTION_EXTERNAL_DEFINE_BEGIN(type, start_version, allocator)

#define GAFF_REFLECTION_DEFINE_END(type, ...) }

#define GAFF_REFLECTION_BUILDER_BEGIN(type) \
	template <class ReflectionBuilder> \
	void GAFF_REFLECTION_NAMESPACE::Reflection<type>::BuildReflection(ReflectionBuilder& builder) \
	{ \
		builder

#define GAFF_REFLECTION_BUILDER_END(type) \
		; \
		builder.finish(); \
	}

#define GAFF_REFLECTION_CLASS_DEFINE(type, allocator) \
	GAFF_REFLECTION_CLASS_DEFINE_BEGIN(type, allocator) \
	GAFF_REFLECTION_CLASS_DEFINE_END(type, allocator)

#define GAFF_REFLECTION_CLASS_DEFINE_BEGIN(type, allocator) \
	const Gaff::ReflectionDefinition<type, allocator>& type::GetReflectionDefinition(void) \
	{ \
		return GAFF_REFLECTION_NAMESPACE::Reflection<type>::GetReflectionDefinition(); \
	} \
	template <class ReflectionBuilder> \
	void type::BuildReflection(ReflectionBuilder& builder) \
	{ \
		builder

#define GAFF_REFLECTION_CLASS_DEFINE_END(type, ...) GAFF_REFLECTION_BUILDER_END(type)


// Template Reflection
#define GAFF_TEMPLATE_REFLECTION_DECLARE_SERIALIZE(type, allocator, ...) \
namespace GAFF_REFLECTION_NAMESPACE { \
	template < GAFF_FOR_EACH_COMMA(GAFF_TEMPLATE_REFLECTION_CLASS, __VA_ARGS__) > \
	class Reflection< type<__VA_ARGS__> > final : public Gaff::IReflection \
	{ \
	public: \
		constexpr static bool HasReflection = true; \
		constexpr static bool HasClassReflection = false; \
		Reflection(void) \
		{ \
			Gaff::AddToReflectionChain(this); \
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
		void init(void) override \
		{ \
			Init(); \
		} \
		static Reflection< type<__VA_ARGS__> >& GetInstance(void) \
		{ \
			return g_instance; \
		} \
		static const char* GetName(void) \
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
		constexpr static Gaff::Hash64 GetHash(void) \
		{ \
			return Gaff::CalcTemplateHash<__VA_ARGS__>(Gaff::FNV1aHash64Const(#type)); \
		} \
		static Gaff::Hash64 GetVersion(void) \
		{ \
			return g_version.getHash(); \
		} \
		static const Gaff::ReflectionDefinition<type<__VA_ARGS__>, allocator>& GetReflectionDefinition(void) \
		{ \
			return g_reflection_definition; \
		} \
		static void Load(const Gaff::ISerializeReader& reader, type<__VA_ARGS__>& object) \
		{ \
			g_instance.load(reader, &object); \
		} \
		static void Save(Gaff::ISerializeWriter& writer, const type<__VA_ARGS__>& object) \
		{ \
			g_instance.save(writer, &object); \
		} \
		static void Init(void); \
		void load(const Gaff::ISerializeReader& reader, void* object) const override; \
		void save(Gaff::ISerializeWriter& writer, const void* object) const override; \
	private: \
		static Gaff::ReflectionDefinition<type<__VA_ARGS__>, allocator> g_reflection_definition; \
		static Gaff::ReflectionVersion< type<__VA_ARGS__> > g_version; \
		static Reflection< type<__VA_ARGS__> > g_instance; \
	}; \
}

#define GAFF_TEMPLATE_REFLECTION_DEFINE_SERIALIZE_INIT(type, serialize_version, allocator, ...) \
	template < GAFF_FOR_EACH_COMMA(GAFF_TEMPLATE_REFLECTION_CLASS, __VA_ARGS__) > \
	Gaff::ReflectionDefinition<type<__VA_ARGS__>, allocator> GAFF_REFLECTION_NAMESPACE::Reflection< type<__VA_ARGS__> >::g_reflection_definition; \
	template < GAFF_FOR_EACH_COMMA(GAFF_TEMPLATE_REFLECTION_CLASS, __VA_ARGS__) > \
	Gaff::ReflectionVersion< type<__VA_ARGS__> > GAFF_REFLECTION_NAMESPACE::Reflection< type<__VA_ARGS__> >::g_version = Gaff::ReflectionVersion< type<__VA_ARGS__> >(serialize_version); \
	template < GAFF_FOR_EACH_COMMA(GAFF_TEMPLATE_REFLECTION_CLASS, __VA_ARGS__) > \
	GAFF_REFLECTION_NAMESPACE::Reflection< type<__VA_ARGS__> > GAFF_REFLECTION_NAMESPACE::Reflection< type<__VA_ARGS__> >::g_instance; \
	template < GAFF_FOR_EACH_COMMA(GAFF_TEMPLATE_REFLECTION_CLASS, __VA_ARGS__) > \
	void GAFF_REFLECTION_NAMESPACE::Reflection< type<__VA_ARGS__> >::Init(void)

#define GAFF_TEMPLATE_REFLECTION_DEFINE_SERIALIZE_LOAD(type, ...) \
	template < GAFF_FOR_EACH_COMMA(GAFF_TEMPLATE_REFLECTION_CLASS, __VA_ARGS__) > \
	void GAFF_REFLECTION_NAMESPACE::Reflection< type<__VA_ARGS__> >::load(const Gaff::ISerializeReader& reader, void* object) const

#define GAFF_TEMPLATE_REFLECTION_DEFINE_SERIALIZE_SAVE(type, ...) \
	template < GAFF_FOR_EACH_COMMA(GAFF_TEMPLATE_REFLECTION_CLASS, __VA_ARGS__) > \
	void GAFF_REFLECTION_NAMESPACE::Reflection< type<__VA_ARGS__> >::save(Gaff::ISerializeWriter& writer, const void* object) const


#define GAFF_TEMPLATE_REFLECTION_DECLARE(type, allocator, ...) \
namespace GAFF_REFLECTION_NAMESPACE { \
	template < GAFF_FOR_EACH_COMMA(GAFF_TEMPLATE_REFLECTION_CLASS, __VA_ARGS__) > \
	GAFF_REFLECTION_DECLARE_COMMON(GAFF_SINGLE_ARG(type<__VA_ARGS__>), allocator, __VA_ARGS__) \
	constexpr static Gaff::Hash64 GetHash(void) \
	{ \
		return Gaff::CalcTemplateHash<__VA_ARGS__>(Gaff::FNV1aHash64Const(#type)); \
	} \
	static const char* GetName(void) \
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
		virtual const Gaff::IReflectionDefinition& getReflectionDefinition(void) const \
		{ \
			return GetReflectionDefinition(); \
		} \
		virtual const void* getBasePointer(void) const \
		{ \
			return this; \
		} \
		virtual void* getBasePointer(void) \
		{ \
			return this; \
		} \
		static const Gaff::ReflectionDefinition<type<__VA_ARGS__>, allocator>& GetReflectionDefinition(void); \
		template <class ReflectionBuilder> \
		static void BuildReflection(ReflectionBuilder& builder)

#define GAFF_TEMPLATE_REFLECTION_DEFINE_BASE(type, start_version, allocator, ...) \
	template < GAFF_FOR_EACH_COMMA(GAFF_TEMPLATE_REFLECTION_CLASS, __VA_ARGS__) > \
	Gaff::ReflectionVersion< type<__VA_ARGS__> > GAFF_REFLECTION_NAMESPACE::Reflection< type<__VA_ARGS__> >::g_version = Gaff::ReflectionVersion< type<__VA_ARGS__> >(start_version); \
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
	GAFF_TEMPLATE_REFLECTION_DEFINE_BEGIN(type, allocator, __VA_ARGS__) \
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
	void GAFF_REFLECTION_NAMESPACE::Reflection<type>::BuildReflection(ReflectionBuilder& builder) \
	{ \
		builder

#define GAFF_TEMPLATE_REFLECTION_BUILDER_END GAFF_REFLECTION_BUILDER_END

#define GAFF_TEMPLATE_REFLECTION_CLASS_DEFINE(type, allocator, ...) \
	GAFF_TEMPLATE_REFLECTION_CLASS_DEFINE_BEGIN(type, allocator, __VA_ARGS__) \
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

#define GAFF_REFLECTION_DECLARE_DEFAULT_AND_POD() \
	template <class T> \
	class Reflection final : public Gaff::IReflection \
	{ \
	public: \
		constexpr static bool HasReflection = false; \
		constexpr static bool HasClassReflection = false; \
		Reflection(void) \
		{ \
			GAFF_ASSERT_MSG(false, "Unknown object type."); \
		} \
		void load(const Gaff::ISerializeReader& /*reader*/, void* /*object*/) const override \
		{ \
			GAFF_ASSERT_MSG(false, "Unknown object type."); \
		} \
		void save(Gaff::ISerializeWriter& /*writer*/, const void* /*object*/) const override \
		{ \
			GAFF_ASSERT_MSG(false, "Unknown object type."); \
		} \
		const char* getName(void) const override \
		{ \
			GAFF_ASSERT_MSG(false, "Unknown object type."); \
			return "Unknown"; \
		} \
		Gaff::Hash64 getHash(void) const override \
		{ \
			GAFF_ASSERT_MSG(false, "Unknown object type."); \
			return 0; \
		} \
		Gaff::Hash64 getVersion(void) const override \
		{ \
			GAFF_ASSERT_MSG(false, "Unknown object type."); \
			return 0; \
		} \
		static bool IsDefined(void) \
		{ \
			return false; \
		} \
		static void RegisterOnDefinedCallback(const eastl::function<void (void)>&) \
		{ \
		} \
		static void RegisterOnDefinedCallback(eastl::function<void (void)>&&) \
		{ \
		} \
	}; \
	GAFF_POD_SERIALIZABLE(int8_t, Int8); \
	GAFF_POD_SERIALIZABLE(int16_t, Int16); \
	GAFF_POD_SERIALIZABLE(int32_t, Int32); \
	GAFF_POD_SERIALIZABLE(int64_t, Int64); \
	GAFF_POD_SERIALIZABLE(uint8_t, UInt8); \
	GAFF_POD_SERIALIZABLE(uint16_t, UInt16); \
	GAFF_POD_SERIALIZABLE(uint32_t, UInt32); \
	GAFF_POD_SERIALIZABLE(uint64_t, UInt64); \
	GAFF_POD_SERIALIZABLE(float, Float); \
	GAFF_POD_SERIALIZABLE(double, Double)

#define GAFF_REFLECTION_DEFINE_POD() \
	Reflection<int8_t> Reflection<int8_t>::g_instance; \
	Reflection<int16_t> Reflection<int16_t>::g_instance; \
	Reflection<int32_t> Reflection<int32_t>::g_instance; \
	Reflection<int64_t> Reflection<int64_t>::g_instance; \
	Reflection<uint8_t> Reflection<uint8_t>::g_instance; \
	Reflection<uint16_t> Reflection<uint16_t>::g_instance; \
	Reflection<uint32_t> Reflection<uint32_t>::g_instance; \
	Reflection<uint64_t> Reflection<uint64_t>::g_instance; \
	Reflection<float> Reflection<float>::g_instance; \
	Reflection<double> Reflection<double>::g_instance



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

template <class T, class Base>
const T* ReflectionCast(const Base& object)
{
	return object.getReflectionDefinition().getInterface<T>(object.getBasePointer());
}

template <class T, class Base>
T* ReflectionCast(Base& object)
{
	return const_cast<IReflectionDefinition&>(object.getReflectionDefinition()).getInterface<T>(object.getBasePointer());
}





template <class... T>
int32_t GetNumArgs(void)
{
	return sizeof...(T);
}

template <class...>
struct CalcTemplateHashHelper;

template <class First, class... Rest>
struct CalcTemplateHashHelper<First, Rest...>
{
	constexpr static Hash32 Hash(Hash32 init)
	{
		return CalcTemplateHashHelper<Rest...>::Hash(HashHelper<std::is_void<First>::value>::Hash<First>(init));
	}

	constexpr static Hash64 Hash(Hash64 init)
	{
		return CalcTemplateHashHelper<Rest...>::Hash(HashHelper<std::is_void<First>::value>::Hash<First>(init));
	}

private:
	template <bool condition>
	struct StringConditionHash;

	template <>
	struct StringConditionHash<true>
	{
		template <size_t size>
		constexpr static Hash32 Hash(const char (&string)[size], Hash32 init)
		{
			return FNV1aHash32Const(string, init);
		}

		template <size_t size>
		constexpr static Hash64 Hash(const char (&string)[size], Hash64 init)
		{
			return FNV1aHash64Const(string, init);
		}
	};

	template <>
	struct StringConditionHash<false>
	{
		template <size_t size>
		constexpr static Hash32 Hash(const char (&)[size], Hash32 init)
		{
			return init;
		}

		template <size_t size>
		constexpr static Hash64 Hash(const char (&)[size], Hash64 init)
		{
			return init;
		}
	};

	template <bool is_void>
	struct HashHelper;

	template <>
	struct HashHelper<true>
	{
		template <class T>
		constexpr static Hash32 Hash(Hash32 init)
		{
			return FNV1aHash32Const("void", init);
		}

		template <class T>
		constexpr static Hash64 Hash(Hash64 init)
		{
			return FNV1aHash64Const("void", init);
		}
	};

	template <>
	struct HashHelper<false>
	{
		template <class T>
		constexpr static Hash32 Hash(Hash32 init)
		{
			using NoPtr = std::remove_pointer<T>::type;
			using NoRef = std::remove_reference<NoPtr>::type;
			using V = std::remove_const<NoRef>::type;

			// This may be ugly as sin, but if you didn't see the struggle
			// I had to endure to find a crazy VC++ compiler bug just to arrive at this code.
			// Thanks for issuing no warnings/errors about bad constexpr functions Microsoft.
			return StringConditionHash<std::is_pointer<T>::value>::Hash("*",
				StringConditionHash<std::is_reference<NoPtr>::value>::Hash("&",
					FNV1aHash32StringConst(GAFF_REFLECTION_NAMESPACE::Reflection<V>::GetName(),
						StringConditionHash<std::is_const<NoRef>::value>::Hash("const", init)
					)
				)
			);
		}

		template <class T>
		constexpr static Hash64 Hash(Hash64 init)
		{
			using NoPtr = std::remove_pointer<T>::type;
			using NoRef = std::remove_reference<NoPtr>::type;
			using V = std::remove_const<NoRef>::type;

			// This may be ugly as sin, but if you didn't see the struggle
			// I had to endure to find a crazy VC++ compiler bug just to arrive at this code.
			// Thanks for issuing no warnings/errors about bad constexpr functions Microsoft.
			return StringConditionHash<std::is_pointer<T>::value>::Hash("*",
				StringConditionHash<std::is_reference<NoPtr>::value>::Hash("&",
					FNV1aHash64StringConst(GAFF_REFLECTION_NAMESPACE::Reflection<V>::GetName(),
						StringConditionHash<std::is_const<NoRef>::value>::Hash("const", init)
					)
				)
			);
		}
	};
};

template <>
struct CalcTemplateHashHelper<>
{
	constexpr static Hash32 Hash(Hash32 init)
	{
		return FNV1aHash32Const("void", init);
	}

	constexpr static Hash64 Hash(Hash64 init)
	{
		return FNV1aHash64Const("void", init);
	}
};

template <class... T>
constexpr Hash32 CalcTemplateHash(Hash32 init)
{
	return CalcTemplateHashHelper<T...>::Hash(init);
}

template <class... T>
constexpr Hash64 CalcTemplateHash(Hash64 init)
{
	return CalcTemplateHashHelper<T...>::Hash(init);
}

void AddToAttributeReflectionChain(IReflection* reflection);
IReflection* GetAttributeReflectionChainHead(void);

void AddToReflectionChain(IReflection* reflection);
IReflection* GetReflectionChainHead(void);

NS_END
