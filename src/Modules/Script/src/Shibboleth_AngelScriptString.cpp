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

#include "Shibboleth_AngelScriptString.h"
#include "Shibboleth_ScriptDefines.h"
#include <Containers/Shibboleth_VectorMap.h>
#include <Ptrs/Shibboleth_SmartPtrs.h>
#include <Shibboleth_HashString.h>
#include <Shibboleth_String.h>
#include "Shibboleth_IncludeAngelScript.h"
#include <charconv>

#define CHECK_RESULT(r) if (r < 0) { return false; }

namespace
{
	static Shibboleth::ProxyAllocator s_allocator{ SCRIPT_ALLOCATOR };

	class StringFactory final : public asIStringFactory
	{
	public:
		~StringFactory(void)
		{
			GAFF_ASSERT(_string_cache.empty());
		}

		const void* GetStringConstant(const char* data, asUINT length) override
		{
			const Shibboleth::HashStringView64<> string_view(reinterpret_cast<const char8_t*>(data), length);

			// The string factory might be modified from multiple
			// threads, so it is necessary to use a mutex.
			asAcquireExclusiveLock();

			auto& entry = _string_cache[string_view.getHash()];

			if (!entry) {
				entry.reset(SHIB_ALLOCT(StringCache, s_allocator));
				entry->string = reinterpret_cast<const char8_t*>(data);
			}

			++entry->count;

			StringCache* const cache = entry.get();

			asReleaseExclusiveLock();

			return reinterpret_cast<const void*>(cache);
		}

		int ReleaseStringConstant(const void *str) override
		{
			if (!str) {
				return asERROR;
			}

			const Shibboleth::HashStringView64<> string_view(reinterpret_cast<const char8_t*>(str));

			int ret = asSUCCESS;

			// The string factory might be modified from multiple
			// threads, so it is necessary to use a mutex.
			asAcquireExclusiveLock();

			const auto it = _string_cache.find(string_view.getHash());

			if (it == _string_cache.end()) {
				ret = asERROR;

			} else {
				--it->second->count;

				if (!it->second->count) {
					_string_cache.erase(it);
				}
			}

			asReleaseExclusiveLock();

			return ret;
		}

		int GetRawStringData(const void *str, char *data, asUINT *length) const override
		{
			if (!str) {
				return asERROR;
			}

			if (length) {
				*length = (asUINT)reinterpret_cast<const Shibboleth::U8String*>(str)->size();
			}

			if (data) {
				memcpy(data, reinterpret_cast<const Shibboleth::U8String*>(str)->data(), reinterpret_cast<const Shibboleth::U8String*>(str)->size());
			}

			return asSUCCESS;
		}

	private:
		struct StringCache final
		{
			Shibboleth::U8String string{ SCRIPT_ALLOCATOR };
			int32_t count = 0;
		};

		Shibboleth::VectorMap< Gaff::Hash64, Shibboleth::UniquePtr<StringCache> > _string_cache{ SCRIPT_ALLOCATOR };
	};

	static StringFactory s_factory;



	static Shibboleth::U8String StringSubString(const Shibboleth::U8String& str, asUINT start, asINT32 count)
	{
		return str.substr(start, static_cast<size_t>(count));
	}

	static size_t StringRFind(const Shibboleth::U8String& str, asINT32 start)
	{
		return str.rfind(str, static_cast<size_t>(start));
	}

	static size_t StringFindLastOf(const Shibboleth::U8String& str, asINT32 start)
	{
		return str.find_last_of(str, static_cast<size_t>(start));
	}

	static size_t StringFindLastNotOf(const Shibboleth::U8String& str, asINT32 start)
	{
		return str.find_last_not_of(str, static_cast<size_t>(start));
	}

	static Shibboleth::U8String& StringErase(Shibboleth::U8String& str, size_t pos, asINT32 count)
	{
		return str.erase(pos, static_cast<size_t>(count));
	}

	static Shibboleth::U8String StringConstructFormatStringFloat(const Shibboleth::U8String& options)
	{
		Shibboleth::U8String fmt{ u8"%", SCRIPT_ALLOCATOR };

		if (options.find(u8'l') != Shibboleth::U8String::npos) {
			fmt += u8'-';
		}

		if (options.find(u8'+') != Shibboleth::U8String::npos) {
			fmt += u8'+';
		}

		if (options.find(u8' ') != Shibboleth::U8String::npos) {
			fmt += u8' ';
		}

		if (options.find(u8'0') != Shibboleth::U8String::npos) {
			fmt += u8'0';
		}

		fmt += u8"*.*";

		if (options.find(u8'e') != Shibboleth::U8String::npos) {
			fmt += u8'e';
		} else if (options.find(u8'E') != Shibboleth::U8String::npos) {
			fmt += u8'E';
		} else {
			fmt += u8'f';
		}

		return fmt;
	}

	static Shibboleth::U8String StringConstructFormatStringInt(const Shibboleth::U8String& options, bool is_unsigned)
	{
		Shibboleth::U8String fmt{ u8"%", SCRIPT_ALLOCATOR };

		if (options.find(u8'l') != Shibboleth::U8String::npos) {
			fmt += u8'-';
		}

		if (options.find(u8'+') != Shibboleth::U8String::npos) {
			fmt += u8'+';
		}

		if (options.find(u8' ') != Shibboleth::U8String::npos) {
			fmt += u8' ';
		}

		if (options.find(u8'0') != Shibboleth::U8String::npos) {
			fmt += u8'0';
		}

	#ifdef _WIN32
		fmt += u8"*I64";
	#else
		#ifdef _LP64
			fmt += u8"*l";
		#else
			fmt += u8"*ll";
		#endif
	#endif

		if (options.find(u8'h') != Shibboleth::U8String::npos) {
			fmt += u8'x';
		} else if (options.find(u8'H') != Shibboleth::U8String::npos) {
			fmt += u8'X';
		} else {
			fmt += (is_unsigned) ? u8'u' : u8'd';
		}

		return fmt;
	}

	template <class T>
	static Shibboleth::U8String StringFormatFloat(T value, const Shibboleth::U8String& options, uint32_t width, uint32_t precision)
	{
		Shibboleth::U8String str{ SCRIPT_ALLOCATOR };

		str.append_sprintf(StringConstructFormatStringFloat(options).data(), width, precision, value);

		return str;
	}

	template <class T>
	static Shibboleth::U8String StringFormatNumber(T value, const Shibboleth::U8String& options, uint32_t width)
	{
		Shibboleth::U8String str{ SCRIPT_ALLOCATOR };

		str.append_sprintf(StringConstructFormatStringInt(options, std::is_unsigned_v<T>).data(), width, value);

		return str;
	}

	template <class T>
	static bool ParseNumber(const Shibboleth::U8String& str, T& result)
	{
	#if __clang__
		if constexpr (std::is_floating_point_v<T>) {
				char* end = nullptr;

				if constexpr (std::is_same_v<T, double>) {
					result = std::strtod(reinterpret_cast<const char*>(str.data()), &end);
					return result != HUGE_VAL;
				} else {
					result = std::strtof(reinterpret_cast<const char*>(str.data()), &end);
					return result != HUGE_VALF;
				}
		} else
	#endif
		{
	 		return std::from_chars(reinterpret_cast<const char*>(str.data()), reinterpret_cast<const char*>(str.data() + str.size()), result).ec == std::errc{};
		}
	}

	static size_t StringNPos(void)
	{
		return Shibboleth::U8String::npos;
	}
}

NS_SHIBBOLETH

bool RegisterScriptString_Native(asIScriptEngine* engine)
{
	int result = engine->RegisterObjectType("U8String", sizeof(U8String), asOBJ_VALUE | asGetTypeTraits<U8String>());
	CHECK_RESULT(result)

	result = engine->RegisterStringFactory("U8String", &s_factory);
	CHECK_RESULT(result)

	result = engine->RegisterObjectBehaviour("U8String", asBEHAVE_CONSTRUCT,  "void f()", asFUNCTION(Gaff::Construct<U8String>), asCALL_CDECL_OBJFIRST); CHECK_RESULT(result)
	result = engine->RegisterObjectBehaviour("U8String", asBEHAVE_CONSTRUCT,  "void f(const U8String&in)", asFUNCTION(GAFF_SINGLE_ARG(Gaff::Construct<U8String, const U8String&>)), asCALL_CDECL_OBJFIRST); CHECK_RESULT(result)
	result = engine->RegisterObjectBehaviour("U8String", asBEHAVE_DESTRUCT,   "void f()", asFUNCTION(Gaff::Deconstruct<U8String>),  asCALL_CDECL_OBJFIRST); CHECK_RESULT(result)

	result = engine->RegisterGlobalFunction("uint64 npos()", asFUNCTION(StringNPos), asCALL_CDECL); CHECK_RESULT(result)

	result = engine->RegisterObjectMethod("U8String", "U8String& opAssign(const U8String&in)", asMETHODPR(U8String, operator=, (const U8String&), U8String&), asCALL_THISCALL); CHECK_RESULT(result)
	result = engine->RegisterObjectMethod("U8String", "U8String& opAddAssign(const U8String&in)", asMETHODPR(U8String, operator+=, (const U8String&), U8String&), asCALL_THISCALL); CHECK_RESULT(result)
	result = engine->RegisterObjectMethod("U8String", "bool opEquals(const U8String&in) const", asFUNCTIONPR(eastl::operator==, (const U8String&, const U8String&), bool), asCALL_CDECL_OBJFIRST); CHECK_RESULT(result)
	result = engine->RegisterObjectMethod("U8String", "int32 opCmp(const U8String&in) const", asMETHODPR(U8String, compare, (const U8String&) const, int), asCALL_THISCALL); CHECK_RESULT(result)
	result = engine->RegisterObjectMethod("U8String", "U8String opAdd(const U8String&in) const", asFUNCTIONPR(eastl::operator+, (const U8String&, const U8String&), U8String), asCALL_CDECL_OBJFIRST); CHECK_RESULT(result)
	result = engine->RegisterObjectMethod("U8String", "const uint8& opIndex(uint) const", asMETHODPR(U8String, at, (size_t) const, const char8_t&), asCALL_THISCALL); CHECK_RESULT(result)
	result = engine->RegisterObjectMethod("U8String", "uint8& opIndex(uint)", asMETHODPR(U8String, at, (size_t), char8_t&), asCALL_THISCALL); CHECK_RESULT(result)

	result = engine->RegisterObjectMethod("U8String", "uint64 size() const", asMETHOD(U8String, size), asCALL_THISCALL); CHECK_RESULT(result)
	result = engine->RegisterObjectMethod("U8String", "uint64 length() const", asMETHOD(U8String, length), asCALL_THISCALL); CHECK_RESULT(result)
	result = engine->RegisterObjectMethod("U8String", "void resize(uint64) const", asMETHODPR(U8String, resize, (size_t), void), asCALL_THISCALL); CHECK_RESULT(result)
	result = engine->RegisterObjectMethod("U8String", "bool empty() const", asMETHOD(U8String, empty), asCALL_THISCALL); CHECK_RESULT(result)

	result = engine->RegisterObjectMethod("U8String", "U8String substr(uint64 start = 0, int32 count = -1) const", asFUNCTION(StringSubString), asCALL_CDECL_OBJFIRST); CHECK_RESULT(result)
	result = engine->RegisterObjectMethod("U8String", "uint64 find(const U8String&in, uint64 start = 0) const", asMETHODPR(U8String, find, (const U8String&, size_t) const, size_t), asCALL_THISCALL); CHECK_RESULT(result)
	result = engine->RegisterObjectMethod("U8String", "uint64 findFirstOf(const U8String&in, uint64 start = 0) const", asMETHODPR(U8String, find_first_of, (const U8String&, size_t) const, size_t), asCALL_THISCALL); CHECK_RESULT(result)
	result = engine->RegisterObjectMethod("U8String", "uint64 findFirstNotOf(const U8String&in, uint64 start = 0) const", asMETHODPR(U8String, find_first_not_of, (const U8String&, size_t) const, size_t), asCALL_THISCALL); CHECK_RESULT(result)
	result = engine->RegisterObjectMethod("U8String", "uint64 rfind(const U8String&in, int32 start = -1) const", asFUNCTION(StringRFind), asCALL_CDECL_OBJFIRST); CHECK_RESULT(result)
	result = engine->RegisterObjectMethod("U8String", "uint64 findLastOf(const U8String&in, int32 start = -1) const", asFUNCTION(StringFindLastOf), asCALL_CDECL_OBJFIRST); CHECK_RESULT(result)
	result = engine->RegisterObjectMethod("U8String", "uint64 findLastNotOf(const U8String&in, int32 start = -1) const", asFUNCTION(StringFindLastNotOf), asCALL_CDECL_OBJFIRST); CHECK_RESULT(result)
	result = engine->RegisterObjectMethod("U8String", "U8String& insert(uint64 pos, const U8String&in other)", asMETHODPR(U8String, insert, (size_t, const U8String&), U8String&), asCALL_THISCALL); CHECK_RESULT(result)
	result = engine->RegisterObjectMethod("U8String", "U8String& erase(uint64 pos, int32 count = -1)", asFUNCTION(StringErase), asCALL_CDECL_OBJFIRST); CHECK_RESULT(result)

	result = engine->RegisterGlobalFunction("U8String FormatNumber(double val, const U8String&in options = \"\", uint32 width = 0, uint32 precision = 0)", asFUNCTION(StringFormatFloat<double>), asCALL_CDECL); CHECK_RESULT(result)
	result = engine->RegisterGlobalFunction("U8String FormatNumber(float val, const U8String&in options = \"\", uint32 width = 0, uint32 precision = 0)", asFUNCTION(StringFormatFloat<float>), asCALL_CDECL); CHECK_RESULT(result)
	result = engine->RegisterGlobalFunction("U8String FormatNumber(uint64 val, const U8String&in options = \"\", uint32 width = 0)", asFUNCTION(StringFormatNumber<uint64_t>), asCALL_CDECL); CHECK_RESULT(result)
	result = engine->RegisterGlobalFunction("U8String FormatNumber(uint32 val, const U8String&in options = \"\", uint32 width = 0)", asFUNCTION(StringFormatNumber<uint32_t>), asCALL_CDECL); CHECK_RESULT(result)
	result = engine->RegisterGlobalFunction("U8String FormatNumber(uint16 val, const U8String&in options = \"\", uint32 width = 0)", asFUNCTION(StringFormatNumber<uint16_t>), asCALL_CDECL); CHECK_RESULT(result)
	result = engine->RegisterGlobalFunction("U8String FormatNumber(uint8 val, const U8String&in options = \"\", uint32 width = 0)", asFUNCTION(StringFormatNumber<uint8_t>), asCALL_CDECL); CHECK_RESULT(result)
	result = engine->RegisterGlobalFunction("U8String FormatNumber(int64 val, const U8String&in options = \"\", uint32 width = 0)", asFUNCTION(StringFormatNumber<int64_t>), asCALL_CDECL); CHECK_RESULT(result)
	result = engine->RegisterGlobalFunction("U8String FormatNumber(int32 val, const U8String&in options = \"\", uint32 width = 0)", asFUNCTION(StringFormatNumber<int32_t>), asCALL_CDECL); CHECK_RESULT(result)
	result = engine->RegisterGlobalFunction("U8String FormatNumber(int16 val, const U8String&in options = \"\", uint32 width = 0)", asFUNCTION(StringFormatNumber<int16_t>), asCALL_CDECL); CHECK_RESULT(result)
	result = engine->RegisterGlobalFunction("U8String FormatNumber(int8 val, const U8String&in options = \"\", uint32 width = 0)", asFUNCTION(StringFormatNumber<int8_t>), asCALL_CDECL); CHECK_RESULT(result)
	result = engine->RegisterGlobalFunction("bool ParseNumber(const U8String&in, double&out)", asFUNCTION(ParseNumber<double>), asCALL_CDECL); CHECK_RESULT(result)
	result = engine->RegisterGlobalFunction("bool ParseNumber(const U8String&in, float&out)", asFUNCTION(ParseNumber<float>), asCALL_CDECL); CHECK_RESULT(result)
	result = engine->RegisterGlobalFunction("bool ParseNumber(const U8String&in, uint64&out)", asFUNCTION(ParseNumber<uint64_t>), asCALL_CDECL); CHECK_RESULT(result)
	result = engine->RegisterGlobalFunction("bool ParseNumber(const U8String&in, uint32&out)", asFUNCTION(ParseNumber<uint32_t>), asCALL_CDECL); CHECK_RESULT(result)
	result = engine->RegisterGlobalFunction("bool ParseNumber(const U8String&in, uint16&out)", asFUNCTION(ParseNumber<uint16_t>), asCALL_CDECL); CHECK_RESULT(result)
	result = engine->RegisterGlobalFunction("bool ParseNumber(const U8String&in, uint8&out)", asFUNCTION(ParseNumber<uint8_t>), asCALL_CDECL); CHECK_RESULT(result)
	result = engine->RegisterGlobalFunction("bool ParseNumber(const U8String&in, int64&out)", asFUNCTION(ParseNumber<int64_t>), asCALL_CDECL); CHECK_RESULT(result)
	result = engine->RegisterGlobalFunction("bool ParseNumber(const U8String&in, int32&out)", asFUNCTION(ParseNumber<int32_t>), asCALL_CDECL); CHECK_RESULT(result)
	result = engine->RegisterGlobalFunction("bool ParseNumber(const U8String&in, int16&out)", asFUNCTION(ParseNumber<int16_t>), asCALL_CDECL); CHECK_RESULT(result)
	result = engine->RegisterGlobalFunction("bool ParseNumber(const U8String&in, int8&out)", asFUNCTION(ParseNumber<int8_t>), asCALL_CDECL); CHECK_RESULT(result)

	return true;
}

NS_END
