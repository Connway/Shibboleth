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

#pragma once

#include "Gaff_Hash.h"

#ifndef GAFF_REFLECTION_NAMESPACE
	#define GAFF_REFLECTION_NAMESPACE Gaff
#endif

#define GAFF_REFLECTION_CLASS_DECLARE_BASE(type, allocator) \
	public: \
		using ThisType = type; \
		constexpr static Gaff::ReflectionHash GetReflectionHash(void) \
		{ \
			return REFL_HASH_CONST(#type); \
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
		static void BuildReflection(ReflectionBuilder& builder); \
		static void ReflectionInit(Gaff::ReflectionDefinition<type, allocator>& ref_def); \
		static void ReflectionInit(Gaff::ReflectionVersion<type>& version); \
	}; \
	template <> \
	class Reflection<type> final : public Gaff::ISerializeInfo \
	{ \
	private: \
		static Reflection<type> g_instance; \
		static bool g_defined; \
		static Gaff::Vector<void (*)(void), allocator> g_on_defined_callbacks; \
		template <class RefT, class RefAllocator> \
		friend class Gaff::ReflectionDefinition; \
	public: \
		constexpr static bool HasReflection = true; \
		const char* getName(void) const override \
		{ \
			return GetName(); \
		} \
		Gaff::ReflectionHash getHash(void) const override \
		{ \
			return GetHash(); \
		} \
		constexpr static Gaff::ReflectionHash GetHash(void) \
		{ \
			return type::GetReflectionHash(); \
		} \
		constexpr static const char* GetName(void) \
		{ \
			return type::GetReflectionName(); \
		} \
		static Reflection<type>& GetInstance(void) \
		{ \
			return g_instance; \
		} \
		static void Init(void)

#define GAFF_REFLECTION_CLASS_DECLARE(type, allocator) \
	GAFF_REFLECTION_CLASS_DECLARE_BASE(type, allocator); \
		void load(Gaff::ISerializeReader& reader, void* object) const override \
		{ \
			GAFF_ASSERT(object); \
			g_reflection_definition.load(reader, object); \
		} \
		void save(Gaff::ISerializeWriter& writer, const void* object) const override \
		{ \
			GAFF_ASSERT(object); \
			g_reflection_definition.save(writer, object); \
		} \
		void Load(Gaff::ISerializeReader& reader, type& object) \
		{ \
			g_reflection_definition.load(reader, object); \
		} \
		void Save(Gaff::ISerializeWriter& writer, const type& object) const \
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
	private: \
		static Gaff::ReflectionDefinition<type, allocator> g_reflection_definition; \

#define GAFF_REFLECTION_CLASS_DEFINE_BEGIN_BASE_BEGIN(type, allocator) \
	Reflection<type> Reflection<type>::g_instance; \
	bool Reflection<type>::g_defined = false; \
	Gaff::Vector<void (*)(void), allocator> Reflection<type>::g_on_defined_callbacks; \
	const Gaff::ReflectionDefinition<type, allocator>& type::GetReflectionDefinition(void) \
	{ \
		return Reflection<type>::GetReflectionDefinition(); \
	} \
	void type::ReflectionInit(Gaff::ReflectionDefinition<type, allocator>& ref_def) \
	{ \
		BuildReflection(ref_def); \
	} \
	void type::ReflectionInit(Gaff::ReflectionVersion<type>& version) \
	{ \
		BuildReflection(version); \
	}

#define GAFF_REFLECTION_CLASS_DEFINE_BEGIN_BASE_END(type) \
	template <class ReflectionBuilder> \
	static void type::BuildReflection(ReflectionBuilder& builder) \
	{ \
		builder

#define GAFF_REFLECTION_CLASS_DEFINE_BEGIN(type, allocator) \
	GAFF_REFLECTION_CLASS_DEFINE_BEGIN_BASE_BEGIN(type, allocator); \
	Gaff::ReflectionDefinition<type, allocator> Reflection<type>::g_reflection_definition; \
	void Reflection<type>::Init(void) \
	{ \
		g_reflection_definition.setUserData(&g_instance); \
		type::ReflectionInit(g_reflection_definition); \
	} \
	GAFF_REFLECTION_CLASS_DEFINE_BEGIN_BASE_END(type)

#define GAFF_REFLECTION_CLASS_DEFINE_END(type) \
		; \
		builder.finish(); \
	}

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
	class Reflection<type> final : public Gaff::ISerializeInfo \
	{ \
	public: \
		constexpr static bool HasReflection = true; \
		void load(Gaff::ISerializeReader& reader, void* object) const override \
		{ \
			GAFF_ASSERT(object); \
			Load(reader, *reinterpret_cast<type*>(object)); \
		} \
		void save(Gaff::ISerializeWriter& writer, const void* object) const override \
		{ \
			GAFF_ASSERT(object); \
			Save(writer, *reinterpret_cast<const type*>(object)); \
		} \
		const char* getName(void) const override \
		{ \
			return GetName(); \
		} \
		Gaff::ReflectionHash getHash(void) const override \
		{ \
			return GetHash(); \
		} \
		static void Load(Gaff::ISerializeReader& reader, type& value) \
		{ \
			value = reader.read##read_write_suffix(); \
		} \
		static void Save(Gaff::ISerializeWriter& writer, type value) \
		{ \
			writer.write##read_write_suffix(value); \
		} \
		constexpr static Gaff::ReflectionHash GetHash(void) \
		{ \
			return REFL_HASH_CONST(#type); \
		} \
		constexpr static const char* GetName(void) \
		{ \
			return #type; \
		} \
		static Reflection<type>& GetInstance(void) \
		{ \
			return g_instance; \
		} \
	private: \
		static Reflection<type> g_instance; \
	}

#define GAFF_REFLECTION_DECLARE_DEFAULT_AND_POD() \
	template <class T> \
	class Reflection final : public Gaff::ISerializeInfo \
	{ \
	public: \
		constexpr static bool HasReflection = false; \
		Reflection(void) \
		{ \
			GAFF_ASSERT_MSG(false, "Unknown object type."); \
		} \
		void load(Gaff::ISerializeReader& /*reader*/, void* /*object*/) const override \
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
			return nullptr; \
		} \
		Gaff::ReflectionHash getHash(void) const override \
		{ \
			GAFF_ASSERT_MSG(false, "Unknown object type."); \
			return 0; \
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

#define RVT_FUNC(type, value) \
	template <> \
	inline ReflectionValueType GetRVT<type>(void) \
	{ \
		return value; \
	}



#define REFL_HASH Gaff::FNV1aHash32
//#define REFL_HASH Gaff::FNV1aHash64

#define REFL_HASH_CONST Gaff::FNV1aHash32Const
//#define REFL_HASH_CONST Gaff::FNV1aHash64Const

#define REFLECTION_CAST_PTR_NAME(T, name, object) \
	reinterpret_cast<T*>( \
		const_cast<Gaff::IReflectionDefinition&>((object)->getReflectionDefinition()).getInterface( \
			REFL_HASH_CONST(name), (object)->getBasePointer() \
		) \
	)

#define REFLECTION_CAST_PTR(T, object) \
	reinterpret_cast<T*>( \
		const_cast<Gaff::IReflectionDefinition&>((object)->getReflectionDefinition()).getInterface( \
			REFL_HASH_CONST(#T), (object)->getBasePointer() \
		) \
	)

#define REFLECTION_CAST(T, object) *REFLECTION_CAST_PTR(T, &object)

NS_GAFF

using ReflectionHash = Hash32;
//using ReflectionHash = Hash64;

using ReflectionHashFunc = ReflectionHash (*)(const char*, size_t);

template <class T, class Base>
const T* ReflectionCast(const Base& object)
{
	return object.getReflectionDefinition().getInterface<T>(&object.getBasePointer());
}

template <class T, class Base>
T* ReflectionCast(Base& object)
{
	return const_cast<Gaff::IReflectionDefinition&>(object.getReflectionDefinition()).getInterface<T>(object.getBasePointer());
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
inline ReflectionValueType GetRVT(void)
{
	if (std::is_enum<T>()) {
		return VT_ENUM;
	}
	else if (std::is_class<T>()) {
		return VT_OBJECT;
	}

	return VT_SIZE;
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

NS_END
