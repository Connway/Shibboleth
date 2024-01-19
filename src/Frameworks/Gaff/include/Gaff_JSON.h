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

#pragma once

#include "Gaff_Platform.h"

#include "Gaff_IncludeRapidJSON.h"
#include <rapidjson/stringbuffer.h>
#include <rapidjson/document.h>

NS_GAFF

class JSON final
{
public:
	using JSONAlloc = void* (*)(size_t);
	using JSONFree = void (*)(void*);

	template <class Callback>
	bool forEachInObject(Callback&& callback) const
	{
		GAFF_ASSERT(isObject());
		const JSONValue& value = getValue();
		auto beg = value.MemberBegin();
		auto end = value.MemberEnd();

		for ( ; beg != end; ++beg) {
			const JSON json(beg->value);

			if (callback(beg->name.GetString(), json)) {
				return true;
			}
		}

		return false;
	}

	template <class Callback>
	bool forEachInArray(Callback&& callback) const
	{
		GAFF_ASSERT(isArray());
		const JSONValue& value = getValue();
		auto beg = value.Begin();
		auto end = value.End();

		for (int32_t index = 0; beg != end; ++beg, ++index) {
			const JSON json(*beg);

			if (callback(index, json)) {
				return true;
			}
		}

		return false;
	}

	static void SetMemoryFunctions(JSONAlloc alloc_func, JSONFree free_func);
	static JSON CreateArray(void);
	static JSON CreateObject(void);
	static JSON CreateInt32(int32_t val);
	static JSON CreateUInt32(uint32_t val);
	static JSON CreateInt64(int64_t val);
	static JSON CreateUInt64(uint64_t val);
	static JSON CreateDouble(double val);
	static JSON CreateStringRef(const char8_t* val);
	static JSON CreateString(const char8_t* val);
	static JSON CreateBool(bool val);
	static JSON CreateTrue(void);
	static JSON CreateFalse(void);
	static JSON CreateNull(void);

	JSON(const JSON& json);
	JSON(JSON&& json);
	JSON(void);
	~JSON(void);

	bool validateFile(const char8_t* schema_file) const;
	bool validate(const JSON& schema) const;
	bool validate(const char8_t* schema_input) const;

	bool parseFile(const char8_t* filename, const JSON& schema);
	bool parseFile(const char8_t* filename, const char8_t* schema_input);
	bool parseFile(const char8_t* filename);
	bool parse(const char8_t* input, const JSON& schema_object);
	bool parse(const char8_t* input, const char8_t* schema_input);
	bool parse(const char8_t* input);

	bool dumpToFile(const char8_t* filename) const;
	const char8_t* dump(char8_t* buffer, int32_t size);
	const char8_t* dump(void);
	void freeDumpString(const char8_t* string);

	bool isObject(void) const;
	bool isArray(void) const;
	bool isString(void) const;
	bool isNumber(void) const;
	bool isInt8(void) const;
	bool isUInt8(void) const;
	bool isInt16(void) const;
	bool isUInt16(void) const;
	bool isInt32(void) const;
	bool isUInt32(void) const;
	bool isInt64(void) const;
	bool isUInt64(void) const;
	bool isFloat(void) const;
	bool isDouble(void) const;
	bool isBool(void) const;
	bool isTrue(void) const;
	bool isFalse(void) const;
	bool isNull(void) const;

	JSON getObject(const char8_t* key) const;
	JSON getObject(const char* key) const;
	JSON getObject(int32_t index) const;

	const char8_t* getKey(char8_t* buffer, size_t buf_size, int32_t index) const;
	const char8_t* getKey(int32_t index) const;
	JSON getValue(int32_t index) const;

	const char8_t* getString(char8_t* buffer, size_t buf_size, const char8_t* default_value) const;
	const char8_t* getString(const char8_t* default_value) const;
	int8_t getInt8(int8_t default_value) const;
	uint8_t getUInt8(uint8_t default_value) const;
	int16_t getInt16(int16_t default_value) const;
	uint16_t getUInt16(uint16_t default_value) const;
	int32_t getInt32(int32_t default_value) const;
	uint32_t getUInt32(uint32_t default_value) const;
	int64_t getInt64(int64_t default_value) const;
	uint64_t getUInt64(uint64_t default_value) const;
	float getFloat(float default_value) const;
	double getDouble(double default_value) const;
	double getNumber(double default_value) const;
	bool getBool(bool default_value) const;

	const char8_t* getString(char8_t* buffer, size_t buf_size) const;
	const char8_t* getString(void) const;
	int8_t getInt8(void) const;
	uint8_t getUInt8(void) const;
	int16_t getInt16(void) const;
	uint16_t getUInt16(void) const;
	int32_t getInt32(void) const;
	uint32_t getUInt32(void) const;
	int64_t getInt64(void) const;
	uint64_t getUInt64(void) const;
	float getFloat(void) const;
	double getDouble(void) const;
	double getNumber(void) const;
	bool getBool(void) const;

	void setObject(const char8_t* key, const JSON& json);
	void setObject(const char8_t* key, JSON&& json);
	void setObject(int32_t index, const JSON& json);
	void setObject(int32_t index, JSON&& json);
	void push(const JSON& json);
	void push(JSON&& json);

	void freeString(const char8_t*) const {}
	int32_t size(void) const;

	const char8_t* getErrorText(void) const;
	const char8_t* getSchemaErrorText(void) const;
	const char8_t* getSchemaKeywordText(void) const;

	JSON& operator=(const JSON& rhs);
	JSON& operator=(JSON&& rhs);

	JSON& operator=(const char8_t* value);
	JSON& operator=(const char* value);
	JSON& operator=(int32_t value);
	JSON& operator=(uint32_t value);
	JSON& operator=(int64_t value);
	JSON& operator=(uint64_t value);
	JSON& operator=(double value);

	bool operator==(const JSON& rhs) const;
	bool operator!=(const JSON& rhs) const;

private:
	class JSONAllocator;

	// This allocator is stateful, so cross EXE/DLL boundaries will produce memory dereference crashes.
	//using JSONInternalAllocator = rapidjson::MemoryPoolAllocator<JSONAllocator>;
	using JSONInternalAllocator = JSONAllocator;
	using JSONDocument = rapidjson::GenericDocument<rapidjson::UTF8<char8_t>, JSONInternalAllocator, JSONAllocator>;
	using JSONValue = rapidjson::GenericValue<rapidjson::UTF8<char8_t>, JSONInternalAllocator>;
	using JSONStringBuffer = rapidjson::GenericStringBuffer<rapidjson::UTF8<char8_t>, JSONAllocator>;

	union
	{
		JSONValue* _value_ref;
		JSONValue _value;
	};

	mutable rapidjson::ParseResult _error;
	mutable JSONStringBuffer _schema_error;
	mutable JSONStringBuffer _keyword_error;
	bool _is_reference = false;

	static JSONInternalAllocator g_allocator;
	static JSONAlloc g_alloc;
	static JSONFree g_free;

	const JSONValue& getValue(void) const;
	JSONValue& getValue(void);

	explicit JSON(const JSONValue& json);
	explicit JSON(JSONValue* json);
	explicit JSON(JSONValue&& json);
};

NS_END
