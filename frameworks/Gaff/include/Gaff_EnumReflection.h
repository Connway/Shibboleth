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

#include "Gaff_EnumReflectionDefinition.h"

#ifndef GAFF_REFLECTION_NAMESPACE
	#define GAFF_REFLECTION_NAMESPACE Gaff
#endif

namespace GAFF_REFLECTION_NAMESPACE
{
	template <class T>
	class EnumReflection final : public Gaff::IEnumReflection
	{
	public:
		constexpr static bool HasReflection = false;

		EnumReflection(void)
		{
			GAFF_ASSERT_MSG(false, "Unknown enum.");
		}

		void load(Gaff::ISerializeReader& /*reader*/, void* /*object*/) const override
		{
			GAFF_ASSERT_MSG(false, "Unknown enum.");
		}
	
		void save(Gaff::ISerializeWriter& /*writer*/, const void* /*object*/) const override
		{
			GAFF_ASSERT_MSG(false, "Unknown enum.");
		}

		const char* getName(void) const override
		{
			GAFF_ASSERT_MSG(false, "Unknown enum.");
			return nullptr;
		}

		Gaff::Hash64 getHash(void) const override
		{
			GAFF_ASSERT_MSG(false, "Unknown enum.");
			return 0;
		}

		Gaff::Hash64 getVersion(void) const override
		{
			GAFF_ASSERT_MSG(false, "Unknown enum.");
			return 0;
		}

		static bool IsDefined(void)
		{
			return false;
		}
	};
}

#define GAFF_ENUM_REFLECTION_DECLARE_COMMON(type, allocator) \
namespace GAFF_REFLECTION_NAMESPACE \
{ \
	template <> \
	class EnumReflection<type> final : public Gaff::IEnumReflection \
	{ \
	private: \
		static EnumReflection<type> g_instance; \
		static bool g_defined; \
		template <class RefT, class RefAllocator> \
		friend class Gaff::EnumReflectionDefinition; \
	public: \
		constexpr static bool HasReflection = true; \
		constexpr static const char* GetName(void) \
		{ \
			return #type; \
		} \
		constexpr static Gaff::Hash64 GetHash(void) \
		{ \
			return Gaff::FNV1aHash64Const(#type); \
		} \
		static EnumReflection<type>& GetInstance(void) \
		{ \
			return g_instance; \
		} \
		const char* getName(void) const override \
		{ \
			return #type; \
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
		static bool IsDefined(void) \
		{ \
			return g_defined; \
		} \
		template <class ReflectionBuilder> \
		static void BuildReflection(ReflectionBuilder& builder);

#define GAFF_ENUM_REFLECTION_DECLARE_BASE(type, allocator) \
		void load(Gaff::ISerializeReader& reader, void* object) const override \
		{ \
			GAFF_ASSERT(object); \
			g_enum_reflection_definition.load(reader, object); \
		} \
		void save(Gaff::ISerializeWriter& writer, const void* object) const override \
		{ \
			GAFF_ASSERT(object); \
			g_enum_reflection_definition.save(writer, object); \
		} \
		static void Load(Gaff::ISerializeReader& reader, type value) \
		{ \
			g_enum_reflection_definition.load(reader, &value); \
		} \
		static void Save(Gaff::ISerializeWriter& writer, type value) const \
		{ \
			g_enum_reflection_definition.save(writer, &value); \
		} \
		static Gaff::Hash64 GetVersion(void) \
		{ \
			return g_enum_reflection_definition.getVersion(); \
		} \
		static const Gaff::EnumReflectionDefinition<type, allocator> & GetReflectionDefinition() const \
		{ \
			return g_enum_reflection_definition; \
		} \
		static void Init(void) \
		{ \
			BuildReflection(g_enum_reflection_definition); \
		} \
		static void SetAllocator(const allocator& a) \
		{ \
			g_enum_reflection_definition.setAllocator(a); \
		} \
	private: \
		static Gaff::EnumReflectionDefinition<type, allocator> g_enum_reflection_definition; \
	}; \
}

#define GAFF_ENUM_REFLECTION_DECLARE(type, allocator) \
	GAFF_ENUM_REFLECTION_DECLARE_COMMON(type, allocator) \
	GAFF_ENUM_REFLECTION_DECLARE_BASE(type, allocator)

#define GAFF_ENUM_REFLECTION_DEFINE_BASE(type, allocator) \
	GAFF_REFLECTION_NAMESPACE::EnumReflection<type> GAFF_REFLECTION_NAMESPACE::EnumReflection<type>::g_instance; \
	bool GAFF_REFLECTION_NAMESPACE::EnumReflection<type>::g_defined = false

#define GAFF_ENUM_REFLECTION_DEFINE_BEGIN_CUSTOM_INIT(type, allocator) \
	Gaff::EnumReflectionDefinition<type, allocator> GAFF_REFLECTION_NAMESPACE::EnumReflection<type>::g_enum_reflection_definition; \
	GAFF_ENUM_REFLECTION_DEFINE_BASE(type, allocator); \
	void GAFF_REFLECTION_NAMESPACE::EnumReflection<type>::Init() \
	{ \
		BuildReflection(g_enum_reflection_definition);

#define GAFF_ENUM_REFLECTION_DEFINE_END_CUSTOM_INIT(type, allocator) }

#define GAFF_ENUM_REFLECTION_DEFINE_BEGIN(type, allocator) \
	GAFF_ENUM_REFLECTION_DEFINE_BEGIN_CUSTOM_INIT(type, allocator) \
	GAFF_ENUM_REFLECTION_DEFINE_END(type) \
	GAFF_ENUM_REFLECTION_BUILDER_BEGIN(type)

#define GAFF_ENUM_REFLECTION_DEFINE_END GAFF_ENUM_REFLECTION_BUILDER_END

#define GAFF_ENUM_REFLECTION_BUILDER_BEGIN(type) \
	template <class ReflectionBuilder> \
	void GAFF_REFLECTION_NAMESPACE::EnumReflection<type>::BuildReflection(ReflectionBuilder& builder) \
	{ \
		builder

#define GAFF_ENUM_REFLECTION_BUILDER_END(type) \
		; \
		builder.finish(); \
	}


NS_GAFF

void AddToEnumReflectionChain(IEnumReflection* reflection);
IEnumReflection* GetEnumReflectionChainHead(void);

NS_END
