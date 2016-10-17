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

#include "Gaff_IncludeRapidJSON.h"
#include <rapidjson/stringbuffer.h>
#include <rapidjson/document.h>

//#ifdef PLATFORM_WINDOWS
//	#pragma warning(push)
//	#pragma warning(disable : 4706)
//#endif

NS_GAFF

class JSON
{
public:
	using JSONAlloc = void* (*)(size_t);
	using JSONFree = void (*)(void*);

	template <class Callback>
	bool forEachInObject(Callback&& callback) const
	{
		GAFF_ASSERT(_value.IsObject());
		auto beg = _value.MemberBegin();
		auto end = _value.MemberEnd();

		for ( ; beg != end; ++beg) {
			JSON json(beg->value);

			if (callback(beg->name.GetString(), json)) {
				return true;
			}
		}

		return false;
	}

	template <class Callback>
	bool forEachInArray(Callback&& callback) const
	{
		GAFF_ASSERT(_value.IsArray());
		auto beg = _value.Begin();
		auto end = _value.End();

		for (size_t index = 0; beg != end; ++beg, ++index) {
			JSON json(*beg);

			if (callback(index, json)) {
				return true;
			}
		}

		return false;
	}

	static INLINE void SetMemoryFunctions(JSONAlloc alloc_func, JSONFree free_func);
	static INLINE JSON CreateArray(void);
	static INLINE JSON CreateObject(void);
	static INLINE JSON CreateInt(int val);
	static INLINE JSON CreateUInt(unsigned int val);
	static INLINE JSON CreateInt64(int64_t val);
	static INLINE JSON CreateUInt64(uint64_t val);
	static INLINE JSON CreateDouble(double val);
	static INLINE JSON CreateString(const char* val);
	static INLINE JSON CreateBool(bool val);
	static INLINE JSON CreateTrue(void);
	static INLINE JSON CreateFalse(void);
	static INLINE JSON CreateNull(void);

	JSON(const JSON& json);
	JSON(JSON&& json);
	JSON(void);
	~JSON(void);

	bool validateFile(const char* schema_file) const;
	bool validate(const JSON& schema) const;
	bool validate(const char* schema_input) const;

	bool parseFile(const char* filename, const JSON& schema);
	bool parseFile(const char* filename, const char* schema_input);
	bool parseFile(const char* filename);
	bool parse(const char* input, const JSON& schema_object);
	bool parse(const char* input, const char* schema_input);
	bool parse(const char* input);
	bool dumpToFile(const char* filename);
	char* dump(void);

	INLINE bool isObject(void) const;
	INLINE bool isArray(void) const;
	INLINE bool isString(void) const;
	INLINE bool isNumber(void) const;
	INLINE bool isInt(void) const;
	INLINE bool isUInt(void) const;
	INLINE bool isInt64(void) const;
	INLINE bool isUInt64(void) const;
	INLINE bool isDouble(void) const;
	INLINE bool isBool(void) const;
	INLINE bool isTrue(void) const;
	INLINE bool isFalse(void) const;
	INLINE bool isNull(void) const;

	INLINE JSON getObject(const char* key) const;
	INLINE JSON getObject(size_t index) const;

	INLINE const char* getString(const char* default_value) const;
	INLINE int getInt(int default_value) const;
	INLINE unsigned int getUInt(unsigned int default_value) const;
	INLINE int64_t getInt64(int64_t default_value) const;
	INLINE uint64_t getUInt64(uint64_t default_value) const;
	INLINE double getDouble(double default_value) const;
	INLINE double getNumber(double default_value) const;
	INLINE bool getBool(bool default_value) const;

	INLINE const char* getString(void) const;
	INLINE int getInt(void) const;
	INLINE unsigned int getUInt(void) const;
	INLINE int64_t getInt64(void) const;
	INLINE uint64_t getUInt64(void) const;
	INLINE double getDouble(void) const;
	INLINE double getNumber(void) const;
	INLINE bool getBool(void) const;

	INLINE void setObject(const char* key, const JSON& json);
	INLINE void setObject(const char* key, JSON&& json);
	INLINE void setObject(size_t index, const JSON& json);
	INLINE void setObject(size_t index, JSON&& json);
	INLINE void push(const JSON& json);
	INLINE void push(JSON&& json);

	INLINE size_t size(void) const;

	INLINE const char* getErrorText(void) const;
	INLINE const char* getSchemaErrorText(void) const;
	INLINE const char* getSchemaKeywordText(void) const;

	INLINE const JSON& operator=(const JSON& rhs);
	INLINE const JSON& operator=(JSON&& rhs);

	INLINE const JSON& operator=(const char* value);
	INLINE const JSON& operator=(int value);
	INLINE const JSON& operator=(unsigned int value);
	INLINE const JSON& operator=(int64_t value);
	INLINE const JSON& operator=(uint64_t value);
	INLINE const JSON& operator=(double value);

	INLINE bool operator==(const JSON& rhs) const;
	INLINE bool operator!=(const JSON& rhs) const;
	INLINE JSON operator[](const char* key) const;
	INLINE JSON operator[](size_t index) const;

private:
	class JSONAllocator;

	// This allocator is stateful, so cross EXE/DLL boundaries will produce memory dereference crashes.
	//using JSONInternalAllocator = rapidjson::MemoryPoolAllocator<JSONAllocator>;
	using JSONInternalAllocator = JSONAllocator;
	using JSONDocument = rapidjson::GenericDocument<rapidjson::UTF8<>, JSONInternalAllocator, JSONAllocator>;
	using JSONValue = rapidjson::GenericValue<rapidjson::UTF8<>, JSONInternalAllocator>;
	using JSONStringBuffer = rapidjson::GenericStringBuffer<rapidjson::UTF8<>, JSONAllocator>;

	JSONValue _value;
	mutable rapidjson::ParseResult _error;
	mutable JSONStringBuffer _schema_error;
	mutable JSONStringBuffer _keyword_error;

	static JSONInternalAllocator g_allocator;
	static JSONAlloc g_alloc;
	static JSONFree g_free;

	explicit JSON(const JSONValue& json);
};

NS_END

//#ifdef PLATFORM_WINDOWS
//	#pragma warning(pop)
//#endif
