/************************************************************************************
Copyright (C) 2020 by Nicholas LaCroix

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

#include <Gaff_Hash.h>

#ifndef GAFF_HASHABLE_NAMESPACE
	#define GAFF_HASHABLE_NAMESPACE Gaff
#endif

#ifndef NS_HASHABLE
	#define NS_HASHABLE namespace GAFF_HASHABLE_NAMESPACE {
#endif


#define GAFF_CLASS_HASHABLE(type) \
	template <> \
	constexpr const char* GetName<type>(void) \
	{ \
		return #type; \
	} \
	template <> \
	constexpr Gaff::Hash64 GetHash<type>(void) \
	{ \
		return Gaff::FNV1aHash64Const(#type); \
	}

#define GAFF_TEMPLATE_CLASS_HASHABLE(type, ...) \
	template < GAFF_FOR_EACH_COMMA(GAFF_TEMPLATE_REFLECTION_CLASS, __VA_ARGS__) > \
	struct TemplateClassHashableHelper< GAFF_SINGLE_ARG(type<__VA_ARGS__>) > final \
	{ \
		static constexpr const char* GetName(void) \
		{ \
			static char s_name[128] = { 0 }; \
			if (!s_name[0]) { \
				char format[64] = { 0 }; \
				int32_t index = snprintf(format, 32, "%s<", #type); \
				for (int32_t i = 0; i < Gaff::GetNumArgs<__VA_ARGS__>(); ++i) { \
					if (i > 0) { \
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
		static constexpr Gaff::Hash64 GetHash(void) \
		{ \
			return Gaff::CalcTemplateHash<__VA_ARGS__>(Gaff::FNV1aHash64Const(#type)); \
		} \
	};


NS_HASHABLE

template <class... T>
struct TemplateClassHashableHelper final
{
	static constexpr const char* GetName(void)
	{
		static_assert(false, "Did not overload GetName() for type.");
		return "ERROR: No Class Name";
	}

	static constexpr Gaff::Hash64 GetHash(void)
	{
		static_assert(false, "Did not overload GetHash() for type.");
		return Gaff::INIT_HASH64;
	}
};

template <class T>
constexpr const char* GetName(void)
{
	return TemplateClassHashableHelper<T>::GetName();
}

template <class T>
constexpr Gaff::Hash64 GetHash(void)
{
	return TemplateClassHashableHelper<T>::GetHash();
}

GAFF_CLASS_HASHABLE(void)

NS_END
