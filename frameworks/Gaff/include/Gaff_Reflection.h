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

#include "Gaff_SerializeInterfaces.h"
#include "Gaff_Assert.h"
#include "Gaff_Hash.h"


#define GAFF_REFLECTION_CLASS_DECLARE_BEGIN(type) \
	template <> \
	class Reflection<type> : public ISerializeInfo \
	{ \
	public: \
		void load(Gaff::ISerializeReader& reader, void* object) override \
		{ \
			GAFF_ASSERT(object); \
			load(reader, *reinterpret_cast<type*>(object)); \
		} \
		void save(Gaff::ISerializeWriter& writer, const void* object) override \
		{ \
			GAFF_ASSERT(object); \
			save(reader, *reinterpret_cast<const type*>(object)); \
		} \
		void load(Gaff::ISerializeReader& reader, type& object) \
		{ \
			g_reflection_definition.load(reader, object); \
		} \
		void save(Gaff::ISerializeWriter& writer, const type& object) const \
		{ \
			g_reflection_definition.save(writer, object); \
		} \
		static const Gaff::IReflectionDefinition* GetReflectionDefinition(void) \
		{ \
			return &g_reflection_definition; \
		} \
		constexpr static Gaff::ReflectionHash GetHash(void) \
		{ \
			REFL_HASH_CONST(#type); \
		} \
		static void Init(void); \
		Gaff::ReflectionDefinition<type> g_reflection_definition; \
	}

#define GAFF_REFLECTION_CLASS_DEFINE_BEGIN(type) \
	Gaff::ReflectionDefinition<type> Reflection<type>::g_reflection_definition; \
	static void Reflection<type>::Init(void) \
	{ \
		g_reflection_definition

#define GAFF_REFLECTION_CLASS_DEFINE_END(type) }

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


#define POD_SERIALIZABLE(type, read_write_suffix) \
	template <> \
	class Reflection<type> : public ISerializeInfo \
	{ \
	public: \
		void load(ISerializeReader& reader, void* object) const override \
		{ \
			GAFF_ASSERT(object); \
			*reinterpret_cast<type*>(object) = reader.read##read_write_suffix(); \
		} \
		void save(ISerializeWriter& writer, const void* object) const override \
		{ \
			GAFF_ASSERT(object); \
			writer.write##read_write_suffix(*reinterpret_cast<const type*>(object)); \
		} \
	}



NS_GAFF

class IReflectionDefinition;

using ReflectionHash = Gaff::Hash32;
//using ReflectionHash = Gaff::Hash64;

using ReflectionHashFunc = ReflectionHash(*)(const char*, size_t);

#define REFL_HASH Gaff::FNV1aHash32
//#define REFL_HASH Gaff::FNV1aHash64

#define REFL_HASH_CONST Gaff::FNV1aHash32
//#define REFL_HASH_CONST Gaff::FNV1aHash64


template <class T>
class Reflection : public ISerializeInfo
{
public:
	void load(ISerializeReader& /*reader*/, void* /*object*/) const override
	{
		GAFF_ASSERT_MSG(false, "Unknown object type.");
	}

	void save(ISerializeWriter& /*writer*/, const void* /*object*/) const override
	{
		GAFF_ASSERT_MSG(false, "Unknown object type.");
	}

	//static void Init(void)
	//{
	//	GAFF_ASSERT_MSG(false, "Unknown object type.");
	//}

	//constexpr static ReflectionHash GetHash(void)
	//{
	//	GAFF_ASSERT_MSG(false, "Unknown object type.");
	//	return 0;
	//}

	//static const IReflectionDefinition* GetReflectionDefinition(void)
	//{
	//	GAFF_ASSERT_MSG(false, "Unknown object type.");
	//	return nullptr;
	//}
};

POD_SERIALIZABLE(int8_t, Int8);
POD_SERIALIZABLE(int16_t, Int16);
POD_SERIALIZABLE(int32_t, Int32);
POD_SERIALIZABLE(int64_t, Int64);
POD_SERIALIZABLE(uint8_t, UInt8);
POD_SERIALIZABLE(uint16_t, UInt16);
POD_SERIALIZABLE(uint32_t, UInt32);
POD_SERIALIZABLE(uint64_t, UInt64);
POD_SERIALIZABLE(float, Float);
POD_SERIALIZABLE(double, Double);

NS_END
