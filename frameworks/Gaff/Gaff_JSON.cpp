/************************************************************************************
Copyright (C) 2017 by Nicholas LaCroix

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

#include "Gaff_JSON.h"
#include "Gaff_File.h"

#ifdef PLATFORM_WINDOWS
	#pragma warning(push)
	#pragma warning(disable : 4127)
#endif

#include <rapidjson/filewritestream.h>
#include <rapidjson/filereadstream.h>
#include <rapidjson/prettywriter.h>
#include <rapidjson/error/en.h>
#include <rapidjson/schema.h>

#ifdef PLATFORM_WINDOWS
	#pragma warning(pop)
#endif

NS_GAFF

class JSON::JSONAllocator
{
public:
	static const bool kNeedFree = true;

	void* Malloc(size_t size)
	{
		return (size) ? g_alloc(size) : nullptr;
	}

	void* Realloc(void* ptr, size_t original_size, size_t new_size)
	{
		if (original_size >= new_size) {
			return ptr;
		}

		if (!ptr) {
			return Malloc(new_size);
		}

		if (!new_size) {
			g_free(ptr);
			return nullptr;
		}


		void* new_ptr = g_alloc(new_size);
		memcpy_s(new_ptr, new_size, ptr, original_size);
		g_free(ptr);

		return new_ptr;
	}

	static void Free(void* ptr)
	{
		g_free(ptr);
	}
};

template <class Writer>
bool WriteJSON(const JSON& json, Writer& writer)
{
	bool success = true;

	if (json.isObject()) {
		success = writer.StartObject();

		if (success) {
			success = !json.forEachInObject([&](const char* key, const JSON& value) -> bool
			{
				success = writer.Key(key);

				if (success) {
					success = WriteJSON(value, writer);
				}

				return !success;
			});
		}

		if (success) {
			success = writer.EndObject();
		}

	} else if (json.isArray()) {
		success = writer.StartArray();

		if (success) {
			success = !json.forEachInArray([&](size_t, const JSON& value) -> bool
			{
				return !WriteJSON(value, writer);
			});
		}

		if (success) {
			success = writer.EndArray();
		}

	} else if (json.isString()) {
		success = writer.String(json.getString(), static_cast<rapidjson::SizeType>(json.size()));

	} else if (json.isInt32()) {
		success = writer.Int(json.getInt32());

	} else if (json.isUInt32()) {
		success = writer.Uint(json.getUInt32());

	} else if (json.isInt64()) {
		success = writer.Int64(json.getInt64());

	} else if (json.isUInt64()) {
		success = writer.Uint64(json.getUInt64());

	} else if (json.isDouble()) {
		success = writer.Double(json.getDouble());

	} else if (json.isBool()) {
		success = writer.Bool(json.getBool());

	} else if (json.isNull()) {
		success = writer.Null();
	}

	return success;
}

JSON::JSONInternalAllocator JSON::g_allocator;
JSON::JSONAlloc JSON::g_alloc = malloc;
JSON::JSONFree JSON::g_free = free;

void JSON::SetMemoryFunctions(JSONAlloc alloc_func, JSONFree free_func)
{
	GAFF_ASSERT(alloc_func && free_func);
	g_alloc = alloc_func;
	g_free = free_func;
}

JSON JSON::CreateArray(void)
{
	JSONValue value = JSONValue(rapidjson::kArrayType);
	return JSON(std::move(value));
}

JSON JSON::CreateObject(void)
{
	JSONValue value = JSONValue(rapidjson::kNumberType);
	return JSON(std::move(value));
}

JSON JSON::CreateInt32(int32_t val)
{
	JSONValue value = JSONValue(val);
	return JSON(std::move(value));
}

JSON JSON::CreateUInt32(uint32_t val)
{
	JSONValue value = JSONValue(val);
	return JSON(std::move(value));
}

JSON JSON::CreateInt64(int64_t val)
{
	JSONValue value = JSONValue(val);
	return JSON(std::move(value));
}

JSON JSON::CreateUInt64(uint64_t val)
{
	JSONValue value = JSONValue(val);
	return JSON(std::move(value));
}

JSON JSON::CreateDouble(double val)
{
	JSONValue value = JSONValue(val);
	return JSON(std::move(value));
}

JSON JSON::CreateString(const char* val)
{
	JSONValue value = JSONValue(val, g_allocator);
	return JSON(std::move(value));
}

JSON JSON::CreateBool(bool val)
{
	JSONValue value = JSONValue(val);
	return JSON(std::move(value));
}

JSON JSON::CreateTrue(void)
{
	JSONValue value = JSONValue(rapidjson::kTrueType);
	return JSON(std::move(value));
}

JSON JSON::CreateFalse(void)
{
	JSONValue value = JSONValue(rapidjson::kFalseType);
	return JSON(std::move(value));
}

JSON JSON::CreateNull(void)
{
	JSONValue value = JSONValue(rapidjson::kNullType);
	return JSON(std::move(value));
}

JSON::JSON(const JSONValue& json):
	_value(json, g_allocator)
{
}

JSON::JSON(const JSON& json):
	_value(json._value, g_allocator)
{
}

JSON::JSON(JSON&& json):
	_value(std::move(json._value))
{
}

JSON::JSON(void)
{
}

JSON::~JSON(void)
{
}

bool JSON::validateFile(const char* schema_file) const
{
	JSON schema;

	if (!schema.parseFile(schema_file)) {
		_error = schema._error;
		return false;
	}

	return validate(schema);
}

bool JSON::validate(const JSON& schema) const
{
	using JSONSchemaDocument = rapidjson::GenericSchemaDocument<JSONValue, JSONInternalAllocator>;
	using JSONSchemaValidator = rapidjson::GenericSchemaValidator<JSONSchemaDocument>;

	JSONSchemaDocument sd(schema._value, nullptr, &g_allocator);
	JSONSchemaValidator validator(sd);

	if (!_value.Accept(validator)) {
		validator.GetInvalidSchemaPointer().StringifyUriFragment(_schema_error);

		const char* keyword_error = validator.GetInvalidSchemaKeyword();
		size_t size = strlen(keyword_error) + 1;
		char* buf = _keyword_error.Push(size);

		memcpy_s(buf, size, keyword_error, size);
		return false;
	}

	return true;
}

bool JSON::validate(const char* schema_input) const
{
	GAFF_ASSERT(schema_input && strlen(schema_input));
	JSON schema;

	if (!schema.parse(schema_input)) {
		_error = schema._error;
		return false;
	}

	return validate(schema);
}

bool JSON::parseFile(const char* filename, const JSON& schema_object)
{
	using JSONSchemaDocument = rapidjson::GenericSchemaDocument<JSONValue, JSONInternalAllocator>;
	using JSONSchemaValidator = rapidjson::GenericSchemaValidator<JSONSchemaDocument>;

	GAFF_ASSERT(filename && strlen(filename));
	FILE* file = nullptr;
	fopen_s(&file, filename, "r");

	if (!file) {
		return false;
	}

	char buffer[2048];

	JSONSchemaDocument sd(schema_object._value, nullptr, &g_allocator);
	JSONSchemaValidator validator(sd);

	rapidjson::FileReadStream stream(file, buffer, 2048);
	rapidjson::Reader reader;

	_error = reader.Parse(stream, validator);

	if (!_error && _error.Code() == rapidjson::kParseErrorTermination) {
		validator.GetInvalidSchemaPointer().StringifyUriFragment(_schema_error);

		const char* keyword_error = validator.GetInvalidSchemaKeyword();
		size_t size = strlen(keyword_error) + 1;
		char* buf = _keyword_error.Push(size);

		memcpy_s(buf, size, keyword_error, size);
	}

	return _error.IsError();
}

bool JSON::parseFile(const char* filename, const char* schema_input)
{
	GAFF_ASSERT(filename && strlen(filename));
	JSON schema;

	if (!schema.parse(schema_input)) {
		_error = schema._error;
		return false;
	}

	return parseFile(filename, schema);
}

bool JSON::parseFile(const char* filename)
{
	GAFF_ASSERT(filename && strlen(filename));
	FILE* file = nullptr;
	fopen_s(&file, filename, "r");

	if (!file) {
		return false;
	}

	char buffer[2048];
	rapidjson::FileReadStream stream(file, buffer, 2048);

	JSONDocument document;
	document.ParseStream<rapidjson::kParseValidateEncodingFlag>(stream);

	fclose(file);

	if (document.HasParseError()) {
		_error.Set(document.GetParseError(), document.GetErrorOffset());
		return false;
	}

	JSONValue& value = document;
	_value = std::move(value);
	return true;
}

bool JSON::parse(const char* input, const JSON& schema)
{
	using JSONSchemaDocument = rapidjson::GenericSchemaDocument<JSONValue, JSONInternalAllocator>;
	using JSONSchemaValidator = rapidjson::GenericSchemaValidator<JSONSchemaDocument>;

	JSONSchemaDocument sd(schema._value, nullptr, &g_allocator);
	JSONSchemaValidator validator(sd);

	rapidjson::StringStream stream(input);
	rapidjson::Reader reader;

	_error = reader.Parse(stream, validator);

	if (!_error && _error.Code() == rapidjson::kParseErrorTermination) {
		validator.GetInvalidSchemaPointer().StringifyUriFragment(_schema_error);
		_schema_error.Push(0);

		const char* keyword_error = validator.GetInvalidSchemaKeyword();
		size_t size = strlen(keyword_error) + 1;
		char* buffer = _keyword_error.Push(size);

		memcpy_s(buffer, size, keyword_error, size);
	}

	return _error.IsError();
}

bool JSON::parse(const char* input, const char* schema_input)
{
	using JSONSchemaDocument = rapidjson::GenericSchemaDocument<JSONValue, JSONInternalAllocator>;
	using JSONSchemaValidator = rapidjson::GenericSchemaValidator<JSONSchemaDocument>;

	JSON schema;

	if (!schema.parse(schema_input)) {
		_error = schema._error;
		return false;
	}

	return parse(input, schema);
}

bool JSON::parse(const char* input)
{
	GAFF_ASSERT(input && strlen(input));

	JSONDocument document;
	document.Parse<rapidjson::kParseValidateEncodingFlag>(input);

	if (document.HasParseError()) {
		_error.Set(document.GetParseError(), document.GetErrorOffset());
		return false;
	}

	JSONValue& value = document;
	_value = std::move(value);
	return true;
}

bool JSON::dumpToFile(const char* filename)
{
	GAFF_ASSERT(_value.IsArray() || _value.IsObject());
	FILE* file = nullptr;
	fopen_s(&file, filename, "w");

	if (!file) {
		return false;
	}

	char buffer[2048];
	rapidjson::FileWriteStream stream(file, buffer, 2048);

	rapidjson::PrettyWriter<
		rapidjson::FileWriteStream, rapidjson::UTF8<>,
		rapidjson::UTF8<>, JSONInternalAllocator
	> writer(stream);

	bool success = WriteJSON(*this, writer);
	fclose(file);

	return success;
}

char* JSON::dump(void)
{
	GAFF_ASSERT(_value.IsArray() || _value.IsObject());
	JSONStringBuffer buffer;

	rapidjson::PrettyWriter<
		JSONStringBuffer, rapidjson::UTF8<>,
		rapidjson::UTF8<>, JSONInternalAllocator
	> writer(buffer);

	if (WriteJSON(*this, writer)) {
		size_t size = buffer.GetSize();
		char* str = reinterpret_cast<char*>(g_alloc(size + 1));

		memcpy_s(str, size, buffer.GetString(), size);
		str[size] = 0;

		return str;
	}

	return nullptr;
}

bool JSON::isObject(void) const
{
	return _value.IsObject();
}

bool JSON::isArray(void) const
{
	return _value.IsArray();
}

bool JSON::isString(void) const
{
	return _value.IsString();
}

bool JSON::isNumber(void) const
{
	return _value.IsNumber();
}

bool JSON::isInt32(void) const
{
	return _value.IsInt();
}

bool JSON::isUInt32(void) const
{
	return _value.IsUint();
}

bool JSON::isInt64(void) const
{
	return _value.IsInt64();
}

bool JSON::isUInt64(void) const
{
	return _value.IsUint64();
}

bool JSON::isFloat(void) const
{
	return _value.IsFloat();
}

bool JSON::isDouble(void) const
{
	return _value.IsDouble();
}

bool JSON::isBool(void) const
{
	return _value.IsBool();
}

bool JSON::isTrue(void) const
{
	return _value.IsTrue();
}

bool JSON::isFalse(void) const
{
	return _value.IsFalse();
}

bool JSON::isNull(void) const
{
	return _value.IsNull();
}

JSON JSON::getObject(const char* key) const
{
	auto it = _value.FindMember(key);

	if (it == _value.MemberEnd()) {
		return CreateNull();
	}

	return JSON(it->value);
}

JSON JSON::getObject(int32_t index) const
{
	return JSON(_value[static_cast<rapidjson::SizeType>(index)]);
}

const char* JSON::getKey(char* buffer, size_t buf_size, int32_t index) const
{
	GAFF_ASSERT(_value.IsObject() && index < size());
	const char* const key = (_value.MemberBegin() + index)->name.GetString();
	strncpy(buffer, key, buf_size);
	return buffer;
}

const char* JSON::getKey(int32_t index) const
{
	GAFF_ASSERT(_value.IsObject() && index < size());
	return (_value.MemberBegin() + index)->name.GetString();
}

JSON JSON::getValue(int32_t index) const
{
	GAFF_ASSERT(_value.IsObject() && index < size());
	return JSON((_value.MemberBegin() + index)->value);
}

const char* JSON::getString(char* buffer, size_t buf_size, const char* default_value) const
{
	return (_value.IsNull()) ? default_value : strncpy(buffer, _value.GetString(), buf_size);
}

const char* JSON::getString(const char* default_value) const
{
	return (_value.IsNull()) ? default_value : _value.GetString();
}

int8_t JSON::getInt8(int8_t default_value) const
{
	return (_value.IsNull()) ? default_value : static_cast<int8_t>(_value.GetInt());
}

uint8_t JSON::getUInt8(uint8_t default_value) const
{
	return (_value.IsNull()) ? default_value : static_cast<uint8_t>(_value.GetUint());
}

int16_t JSON::getInt16(int16_t default_value) const
{
	return (_value.IsNull()) ? default_value : static_cast<int16_t>(_value.GetInt());
}

uint16_t JSON::getUInt16(uint16_t default_value) const
{
	return (_value.IsNull()) ? default_value : static_cast<uint16_t>(_value.GetUint());
}

int32_t JSON::getInt32(int32_t default_value) const
{
	return (_value.IsNull()) ? default_value : _value.GetInt();
}

uint32_t JSON::getUInt32(uint32_t default_value) const
{
	return (_value.IsNull()) ? default_value : _value.GetUint();
}

int64_t JSON::getInt64(int64_t default_value) const
{
	return (_value.IsNull()) ? default_value : _value.GetInt64();
}

uint64_t JSON::getUInt64(uint64_t default_value) const
{
	return (_value.IsNull()) ? default_value : _value.GetUint64();
}

float JSON::getFloat(float default_value) const
{
	return (_value.IsNull()) ? default_value : _value.GetFloat();
}

double JSON::getDouble(double default_value) const
{
	return (_value.IsNull()) ? default_value : _value.GetDouble();
}

double JSON::getNumber(double default_value) const
{
	return (_value.IsNull()) ? default_value : getNumber();
}

bool JSON::getBool(bool default_value) const
{
	return (_value.IsNull()) ? default_value : _value.GetBool();
}

const char* JSON::getString(char* buffer, size_t buf_size) const
{
	return strncpy(buffer, _value.GetString(), buf_size);
}

const char* JSON::getString(void) const
{
	return _value.GetString();
}

int8_t JSON::getInt8(void) const
{
	return static_cast<int8_t>(_value.GetInt());
}

uint8_t JSON::getUInt8(void) const
{
	return static_cast<uint8_t>(_value.GetUint());
}

int16_t JSON::getInt16(void) const
{
	return static_cast<int16_t>(_value.GetInt());
}

uint16_t JSON::getUInt16(void) const
{
	return static_cast<uint16_t>(_value.GetUint());
}

int32_t JSON::getInt32(void) const
{
	return _value.GetInt();
}

uint32_t JSON::getUInt32(void) const
{
	return _value.GetUint();
}

int64_t JSON::getInt64(void) const
{
	return _value.GetInt64();
}

uint64_t JSON::getUInt64(void) const
{
	return _value.GetUint64();
}

float JSON::getFloat(void) const
{
	return _value.GetFloat();
}

double JSON::getDouble(void) const
{
	return _value.GetDouble();
}

double JSON::getNumber(void) const
{
	double value = 0.0f;

	if (_value.IsInt()) {
		value = static_cast<double>(_value.GetInt());
	} else if (_value.IsUint()) {
		value = static_cast<double>(_value.GetUint());
	} else if (_value.IsInt64()) {
		value = static_cast<double>(_value.GetInt64());
	} else if (_value.IsUint64()) {
		value = static_cast<double>(_value.GetUint64());
	}

	return value;
}

bool JSON::getBool(void) const
{
	return _value.GetBool();
}

void JSON::setObject(const char* key, const JSON& json)
{
	GAFF_ASSERT(_value.IsObject());
	JSONValue value(json._value, g_allocator);

	auto it = _value.FindMember(key);

	if (it != _value.MemberEnd()) {
		it->value = std::move(value);
	} else {
		_value.AddMember(JSONValue(key, g_allocator), std::move(value), g_allocator);
	}
}

void JSON::setObject(const char* key, JSON&& json)
{
	GAFF_ASSERT(_value.IsObject());
	auto it = _value.FindMember(key);

	if (it != _value.MemberEnd()) {
		it->value = std::move(json._value);
	} else {
		_value.AddMember(JSONValue(key, g_allocator), std::move(json._value), g_allocator);
	}
}

void JSON::setObject(int32_t index, const JSON& json)
{
	GAFF_ASSERT(_value.IsArray() && index < static_cast<int32_t>(_value.Size()));
	JSONValue value(json._value, g_allocator);

	_value[static_cast<rapidjson::SizeType>(index)] = std::move(value);
}

void JSON::setObject(int32_t index, JSON&& json)
{
	GAFF_ASSERT(_value.IsArray() && index < static_cast<int32_t>(_value.Size()));
	_value[static_cast<rapidjson::SizeType>(index)] = std::move(json._value);
}

void JSON::push(const JSON& json)
{
	GAFF_ASSERT(_value.IsArray());
	JSONValue value(json._value, g_allocator);

	_value.PushBack(std::move(value), g_allocator);
}

void JSON::push(JSON&& json)
{
	GAFF_ASSERT(_value.IsArray());
	_value.PushBack(std::move(json._value), g_allocator);
}

int32_t JSON::size(void) const
{
	GAFF_ASSERT(isString() || isArray() || isObject());

	if (isString()) {
		return static_cast<int32_t>(_value.GetStringLength());
	} else if (isArray()) {
		return static_cast<int32_t>(_value.Size());
	}

	return static_cast<int32_t>(_value.MemberCount());
}

const char* JSON::getErrorText(void) const
{
	return _error.IsError() ? rapidjson::GetParseError_En(_error.Code()) : nullptr;
}

const char* JSON::getSchemaErrorText(void) const
{
	return _schema_error.GetString();
}

const char* JSON::getSchemaKeywordText(void) const
{
	return _keyword_error.GetString();
}

JSON& JSON::operator=(const JSON& rhs)
{
	_value = JSONValue(rhs._value, g_allocator);
	return *this;
}

JSON& JSON::operator=(JSON&& rhs)
{
	_value = std::move(rhs._value);
	return *this;
}

JSON& JSON::operator=(const char* value)
{
	_value.SetString(value, g_allocator);
	return *this;
}

JSON& JSON::operator=(int32_t value)
{
	_value.SetInt(value);
	return *this;
}

JSON& JSON::operator=(uint32_t value)
{
	_value.SetUint(value);
	return *this;
}

JSON& JSON::operator=(int64_t value)
{
	_value.SetInt64(value);
	return *this;
}

JSON& JSON::operator=(uint64_t value)
{
	_value.SetUint64(value);
	return *this;
}

JSON& JSON::operator=(double value)
{
	_value.SetDouble(value);
	return *this;
}

bool JSON::operator==(const JSON& rhs) const
{
	return _value == rhs._value;
}

bool JSON::operator!=(const JSON& rhs) const
{
	return _value != rhs._value;
}

JSON JSON::operator[](const char* key) const
{
	return getObject(key);
}

JSON JSON::operator[](int32_t index) const
{
	return getObject(index);
}

NS_END
