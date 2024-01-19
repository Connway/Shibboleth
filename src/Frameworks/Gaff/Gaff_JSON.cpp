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

#include "Gaff_JSON.h"
#include "Gaff_String.h"
#include "Gaff_File.h"

MSVC_DISABLE_WARNING_PUSH(4127)
#include <rapidjson/filewritestream.h>
#include <rapidjson/filereadstream.h>
#include <rapidjson/prettywriter.h>
#include <rapidjson/error/en.h>
#include <rapidjson/schema.h>
MSVC_DISABLE_WARNING_POP()

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

		void* const new_ptr = g_alloc(new_size);

	#ifdef PLATFORM_WINDOWS
		memcpy_s(new_ptr, new_size, ptr, original_size);
	#else
		memcpy(new_ptr, ptr, (original_size < new_size) ? original_size : new_size);
	#endif

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
			success = !json.forEachInObject([&](const char8_t* key, const JSON& value) -> bool
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
	JSONValue value = JSONValue(rapidjson::kObjectType);
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

JSON JSON::CreateStringRef(const char8_t* val)
{
	JSONValue value = JSONValue(JSONValue::StringRefType(val));
	return JSON(std::move(value));
}

JSON JSON::CreateString(const char8_t* val)
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

const JSON::JSONValue& JSON::getValue(void) const
{
	return const_cast<JSON*>(this)->getValue();
}

JSON::JSONValue& JSON::getValue(void)
{
	return (_is_reference) ? *_value_ref : _value;
}

JSON::JSON(const JSONValue& json):
	_value(json, g_allocator)
{
}

JSON::JSON(JSONValue* json):
	_value_ref(json), _is_reference(true)
{
}

JSON::JSON(JSONValue&& json):
	_value(std::move(json))
{
}

JSON::JSON(const JSON& json):
	JSON()
{
	*this = json;
}

JSON::JSON(JSON&& json):
	JSON()
{
	*this = std::move(json);
}

JSON::JSON(void)
{
	new(&_value) JSONValue();
}

JSON::~JSON(void)
{
	if (!_is_reference) {
		_value.~JSONValue();
	}
}

bool JSON::validateFile(const char8_t* schema_file) const
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

	JSONSchemaDocument sd(schema._value, nullptr, 0, nullptr, &g_allocator);
	JSONSchemaValidator validator(sd);

	if (!getValue().Accept(validator)) {
		validator.GetInvalidSchemaPointer().StringifyUriFragment(_schema_error);

		const char8_t* const keyword_error = validator.GetInvalidSchemaKeyword();
		const size_t size = eastl::CharStrlen(keyword_error) + 1;
		char8_t* const buf = _keyword_error.Push(size);

	#ifdef PLATFORM_WINDOWS
		memcpy_s(buf, size, keyword_error, size);
	#else
		memcpy(buf, keyword_error, size);
	#endif

		return false;
	}

	return true;
}

bool JSON::validate(const char8_t* schema_input) const
{
	GAFF_ASSERT(schema_input && eastl::CharStrlen(schema_input));
	JSON schema;

	if (!schema.parse(schema_input)) {
		_error = schema._error;
		return false;
	}

	return validate(schema);
}

bool JSON::parseFile(const char8_t* filename, const JSON& schema)
{
	using JSONSchemaDocument = rapidjson::GenericSchemaDocument<JSONValue, JSONInternalAllocator>;
	using JSONSchemaValidator = rapidjson::GenericSchemaValidator<JSONSchemaDocument>;

	GAFF_ASSERT(filename && eastl::CharStrlen(filename));
	*this = CreateNull();

	Gaff::File file;

	if (!file.open(filename)) {
		return false;
	}

	// $TODO: Should probably just allocate this dynamically.
	char buffer[2048];

	JSONSchemaDocument sd(schema.getValue(), nullptr, 0, nullptr, &g_allocator);
	JSONSchemaValidator validator(sd);

	rapidjson::FileReadStream stream(file.getFile(), buffer, 2048);
	rapidjson::GenericReader<rapidjson::UTF8<char8_t>, rapidjson::UTF8<char8_t> > reader;

	_error = reader.Parse(stream, validator);

	if (!_error && _error.Code() == rapidjson::kParseErrorTermination) {
		validator.GetInvalidSchemaPointer().StringifyUriFragment(_schema_error);

		const char8_t* const keyword_error = validator.GetInvalidSchemaKeyword();
		const size_t size = eastl::CharStrlen(keyword_error) + 1;
		char8_t* const buf = _keyword_error.Push(size);

	#ifdef PLATFORM_WINDOWS
		memcpy_s(buf, size, keyword_error, size);
	#else
		memcpy(buf, keyword_error, size);
	#endif
	}

	if (_error.IsError()) {
		return false;
	}

	// Shouldn't be an error. Any error should be caught be cauht in the validation stage.
	JSONDocument document;
	document.ParseStream(stream);

	JSONValue& value = document;
	_value = std::move(value);
	_is_reference = false;

	return true;
}

bool JSON::parseFile(const char8_t* filename, const char8_t* schema_input)
{
	JSON schema;

	if (!schema.parse(schema_input)) {
		_error = schema._error;
		return false;
	}

	return parseFile(filename, schema);
}

bool JSON::parseFile(const char8_t* filename)
{
	GAFF_ASSERT(filename && eastl::CharStrlen(filename));
	*this = CreateNull();

	Gaff::File file;

	if (!file.open(filename)) {
		return false;
	}

	// $TODO: Should probably just allocate this dynamically.
	char buffer[2048];
	rapidjson::FileReadStream stream(file.getFile(), buffer, 2048);

	JSONDocument document;
	document.ParseStream(stream);

	if (document.HasParseError()) {
		_error.Set(document.GetParseError(), document.GetErrorOffset());
		return false;
	}

	JSONValue& value = document;
	_value = std::move(value);
	_is_reference = false;
	return true;
}

bool JSON::parse(const char8_t* input, const JSON& schema)
{
	GAFF_ASSERT(input && eastl::CharStrlen(input));
	*this = CreateNull();

	using JSONSchemaDocument = rapidjson::GenericSchemaDocument<JSONValue, JSONInternalAllocator>;
	using JSONSchemaValidator = rapidjson::GenericSchemaValidator<JSONSchemaDocument>;

	JSONSchemaDocument sd(schema.getValue(), nullptr, 0, nullptr, &g_allocator);
	JSONSchemaValidator validator(sd);

	rapidjson::GenericStringStream< rapidjson::UTF8<char8_t> > stream(input);
	rapidjson::GenericReader<rapidjson::UTF8<char8_t>, rapidjson::UTF8<char8_t> > reader;

	_error = reader.Parse(stream, validator);

	if (!_error && _error.Code() == rapidjson::kParseErrorTermination) {
		validator.GetInvalidSchemaPointer().StringifyUriFragment(_schema_error);
		_schema_error.Push(0);

		const char8_t* const keyword_error = validator.GetInvalidSchemaKeyword();
		const size_t size = eastl::CharStrlen(keyword_error) + 1;
		char8_t* const buf = _keyword_error.Push(size);

	#ifdef PLATFORM_WINDOWS
		memcpy_s(buf, size, keyword_error, size);
	#else
		memcpy(buf, keyword_error, size);
	#endif
	}

	if (_error.IsError()) {
		return false;
	}

	// Shouldn't be an error. Any error should be caught be cauht in the validation stage.
	JSONDocument document;
	document.Parse(input);

	JSONValue& value = document;
	_value = std::move(value);
	_is_reference = false;

	return true;
}

bool JSON::parse(const char8_t* input, const char8_t* schema_input)
{
	JSON schema;

	if (!schema.parse(schema_input)) {
		_error = schema._error;
		return false;
	}

	return parse(input, schema);
}

bool JSON::parse(const char8_t* input)
{
	GAFF_ASSERT(input && eastl::CharStrlen(input));
	*this = CreateNull();

	JSONDocument document;
	document.Parse(input);

	if (document.HasParseError()) {
		_error.Set(document.GetParseError(), document.GetErrorOffset());
		return false;
	}

	JSONValue& value = document;
	_value = std::move(value);
	_is_reference = false;
	return true;
}

bool JSON::dumpToFile(const char8_t* filename) const
{
	GAFF_ASSERT(isArray() || isObject());
	Gaff::File file;

	if (!file.open(filename, Gaff::File::OpenMode::Write)) {
		return false;
	}

	char buffer[2048];
	rapidjson::FileWriteStream stream(file.getFile(), buffer, 2048);

	rapidjson::PrettyWriter<
		rapidjson::FileWriteStream, rapidjson::UTF8<char8_t>,
		rapidjson::UTF8<char8_t>, JSONInternalAllocator
	> writer(stream);

	return WriteJSON(*this, writer);
}

const char8_t* JSON::dump(char8_t* buffer, int32_t size)
{
	GAFF_ASSERT(isArray() || isObject());
	JSONStringBuffer string_buffer;

	rapidjson::PrettyWriter<
		JSONStringBuffer, rapidjson::UTF8<char8_t>,
		rapidjson::UTF8<char8_t>, JSONInternalAllocator
	> writer(string_buffer);

	if (WriteJSON(*this, writer)) {
		const size_t str_size = string_buffer.GetSize();
		const size_t min_size = (size < static_cast<int32_t>(str_size)) ? static_cast<size_t>(size - 1) : str_size;

	#ifdef PLATFORM_WINDOWS
		memcpy_s(buffer, size, string_buffer.GetString(), str_size);
	#else
		memcpy(buffer, string_buffer.GetString(), min_size);
	#endif

		buffer[min_size] = 0;

		return buffer;
	}

	return nullptr;
}

const char8_t* JSON::dump(void)
{
	GAFF_ASSERT(isArray() || isObject());
	JSONStringBuffer buffer;

	rapidjson::PrettyWriter<
		JSONStringBuffer, rapidjson::UTF8<char8_t>,
		rapidjson::UTF8<char8_t>, JSONInternalAllocator
	> writer(buffer);

	if (WriteJSON(*this, writer)) {
		size_t size = buffer.GetSize();
		char8_t* const str = reinterpret_cast<char8_t*>(g_alloc(size + 1));

	#ifdef PLATFORM_WINDOWS
		memcpy_s(str, size, buffer.GetString(), size);
#else
		memcpy(str, buffer.GetString(), size);
	#endif

		str[size] = 0;

		return str;
	}

	return nullptr;
}

void JSON::freeDumpString(const char8_t* string)
{
	g_free(const_cast<char8_t*>(string));
}

bool JSON::isObject(void) const
{
	return getValue().IsObject();
}

bool JSON::isArray(void) const
{
	return getValue().IsArray();
}

bool JSON::isString(void) const
{
	return getValue().IsString();
}

bool JSON::isNumber(void) const
{
	return getValue().IsNumber();
}

bool JSON::isInt8(void) const
{
	return getValue().IsInt();
}

bool JSON::isUInt8(void) const
{
	return getValue().IsUint();
}

bool JSON::isInt16(void) const
{
	return getValue().IsInt();
}

bool JSON::isUInt16(void) const
{
	return getValue().IsUint();
}

bool JSON::isInt32(void) const
{
	return getValue().IsInt();
}

bool JSON::isUInt32(void) const
{
	return getValue().IsUint();
}

bool JSON::isInt64(void) const
{
	return getValue().IsInt64();
}

bool JSON::isUInt64(void) const
{
	return getValue().IsUint64();
}

bool JSON::isFloat(void) const
{
	return getValue().IsFloat();
}

bool JSON::isDouble(void) const
{
	return getValue().IsDouble();
}

bool JSON::isBool(void) const
{
	return getValue().IsBool();
}

bool JSON::isTrue(void) const
{
	return getValue().IsTrue();
}

bool JSON::isFalse(void) const
{
	return getValue().IsFalse();
}

bool JSON::isNull(void) const
{
	return getValue().IsNull();
}

JSON JSON::getObject(const char8_t* key) const
{
	auto it = getValue().FindMember(key);

	if (it == getValue().MemberEnd()) {
		return CreateNull();
	}

	return JSON(const_cast<JSONValue*>(&it->value));
}

JSON JSON::getObject(const char* key) const
{
	CONVERT_STRING(char8_t, temp_key, key);
	return getObject(temp_key);
}

JSON JSON::getObject(int32_t index) const
{
	return JSON(const_cast<JSONValue*>(&getValue()[static_cast<rapidjson::SizeType>(index)]));
}

const char8_t* JSON::getKey(char8_t* buffer, size_t buf_size, int32_t index) const
{
	GAFF_ASSERT(isObject() && index < size());

	const char8_t* const key = (getValue().MemberBegin() + index)->name.GetString();
	const size_t len = (buf_size < (eastl::CharStrlen(key) + 1)) ? buf_size : eastl::CharStrlen(key) + 1;

	memcpy(buffer, key, len);
	buffer[buf_size - 1] = 0;

	return buffer;
}

const char8_t* JSON::getKey(int32_t index) const
{
	GAFF_ASSERT(isObject() && index < size());
	return (getValue().MemberBegin() + index)->name.GetString();
}

JSON JSON::getValue(int32_t index) const
{
	GAFF_ASSERT(isObject() && index < size());
	return JSON(const_cast<JSONValue*>(&(getValue().MemberBegin() + index)->value));
}

const char8_t* JSON::getString(char8_t* buffer, size_t buf_size, const char8_t* default_value) const
{
	const char8_t* const str = (isString()) ? getValue().GetString() : default_value;
	const size_t len = (buf_size < (eastl::CharStrlen(str) + 1)) ? buf_size : eastl::CharStrlen(str) + 1;

	memcpy(buffer, str, len);
	buffer[buf_size - 1] = 0;

	return buffer;
}

const char8_t* JSON::getString(const char8_t* default_value) const
{
	return (isString()) ? getValue().GetString() : default_value;
}

int8_t JSON::getInt8(int8_t default_value) const
{
	return (isInt8()) ? getInt8() : default_value ;
}

uint8_t JSON::getUInt8(uint8_t default_value) const
{
	return (isUInt8()) ? getUInt8() : default_value;
}

int16_t JSON::getInt16(int16_t default_value) const
{
	return (isInt16()) ? getInt16() : default_value;
}

uint16_t JSON::getUInt16(uint16_t default_value) const
{
	return (isUInt16()) ? getUInt16() : default_value;
}

int32_t JSON::getInt32(int32_t default_value) const
{
	return (isInt32()) ? getInt32() : default_value;
}

uint32_t JSON::getUInt32(uint32_t default_value) const
{
	return (isUInt32()) ? getUInt32() : default_value;
}

int64_t JSON::getInt64(int64_t default_value) const
{
	return (isInt64()) ? getInt64() : default_value;
}

uint64_t JSON::getUInt64(uint64_t default_value) const
{
	return (isUInt64()) ? getUInt64() : default_value;
}

float JSON::getFloat(float default_value) const
{
	return (isFloat()) ? getFloat() : default_value;
}

double JSON::getDouble(double default_value) const
{
	return (isDouble()) ? getDouble() : default_value;
}

double JSON::getNumber(double default_value) const
{
	return (isNumber()) ? getNumber() : default_value;
}

bool JSON::getBool(bool default_value) const
{
	return (isBool()) ? getBool() : default_value;
}

const char8_t* JSON::getString(char8_t* buffer, size_t buf_size) const
{
	const char8_t* const str = getValue().GetString();
	const size_t len = (buf_size < (eastl::CharStrlen(str) + 1)) ? buf_size : eastl::CharStrlen(str) + 1;

	memcpy(buffer, str, len);
	buffer[buf_size - 1] = 0;

	return buffer;

}

const char8_t* JSON::getString(void) const
{
	return getValue().GetString();
}

int8_t JSON::getInt8(void) const
{
	return static_cast<int8_t>(getValue().GetInt());
}

uint8_t JSON::getUInt8(void) const
{
	return static_cast<uint8_t>(getValue().GetUint());
}

int16_t JSON::getInt16(void) const
{
	return static_cast<int16_t>(getValue().GetInt());
}

uint16_t JSON::getUInt16(void) const
{
	return static_cast<uint16_t>(getValue().GetUint());
}

int32_t JSON::getInt32(void) const
{
	return getValue().GetInt();
}

uint32_t JSON::getUInt32(void) const
{
	return getValue().GetUint();
}

int64_t JSON::getInt64(void) const
{
	return getValue().GetInt64();
}

uint64_t JSON::getUInt64(void) const
{
	return getValue().GetUint64();
}

float JSON::getFloat(void) const
{
	return getValue().GetFloat();
}

double JSON::getDouble(void) const
{
	return getValue().GetDouble();
}

double JSON::getNumber(void) const
{
	const JSONValue& value = getValue();
	double ret = 0.0f;

	if (value.IsInt()) {
		ret = static_cast<double>(value.GetInt());
	} else if (value.IsUint()) {
		ret = static_cast<double>(value.GetUint());
	} else if (value.IsInt64()) {
		ret = static_cast<double>(value.GetInt64());
	} else if (value.IsUint64()) {
		ret = static_cast<double>(value.GetUint64());
	} else if (value.IsFloat()) {
		ret = static_cast<double>(value.GetFloat());
	} else if (value.IsDouble()) {
		ret = value.GetDouble();
	}

	return ret;
}

bool JSON::getBool(void) const
{
	return getValue().GetBool();
}

void JSON::setObject(const char8_t* key, const JSON& json)
{
	GAFF_ASSERT(isObject());
	JSONValue value(json.getValue(), g_allocator);
	JSONValue& value_ref = getValue();

	auto it = value_ref.FindMember(key);

	if (it != value_ref.MemberEnd()) {
		it->value = std::move(value);
	} else {
		value_ref.AddMember(JSONValue(key, g_allocator), std::move(value), g_allocator);
	}
}

void JSON::setObject(const char8_t* key, JSON&& json)
{
	if (json._is_reference) {
		setObject(key, json);
	}

	GAFF_ASSERT(isObject());

	JSONValue& value_ref = getValue();
	auto it = value_ref.FindMember(key);

	if (it != value_ref.MemberEnd()) {
		it->value = std::move(value_ref);
	} else {
		value_ref.AddMember(JSONValue(key, g_allocator), std::move(json._value), g_allocator);
	}
}

void JSON::setObject(int32_t index, const JSON& json)
{
	GAFF_ASSERT(isArray() && index < size());

	JSONValue value(json.getValue(), g_allocator);
	getValue()[static_cast<rapidjson::SizeType>(index)] = std::move(value);
}

void JSON::setObject(int32_t index, JSON&& json)
{
	if (json._is_reference) {
		setObject(index, json);
	}

	GAFF_ASSERT(isArray() && index < size());
	getValue()[static_cast<rapidjson::SizeType>(index)] = std::move(json._value);
}

void JSON::push(const JSON& json)
{
	GAFF_ASSERT(isArray());

	JSONValue value(json.getValue(), g_allocator);
	getValue().PushBack(std::move(value), g_allocator);
}

void JSON::push(JSON&& json)
{
	if (json._is_reference) {
		push(json);
	}

	GAFF_ASSERT(isArray());
	getValue().PushBack(std::move(json._value), g_allocator);
}

int32_t JSON::size(void) const
{
	GAFF_ASSERT(isString() || isArray() || isObject());

	if (isObject()) {
		return static_cast<int32_t>(getValue().MemberCount());
	} else if (isArray()) {
		return static_cast<int32_t>(getValue().Size());
	}

	return static_cast<int32_t>(getValue().GetStringLength());
}

const char8_t* JSON::getErrorText(void) const
{
	return _error.IsError() ? rapidjson::GetParseError_En(_error.Code()) : nullptr;
}

const char8_t* JSON::getSchemaErrorText(void) const
{
	return _schema_error.GetString();
}

const char8_t* JSON::getSchemaKeywordText(void) const
{
	return _keyword_error.GetString();
}

JSON& JSON::operator=(const JSON& rhs)
{
	if (!_is_reference) {
		_value.~JSONValue();
	}

	_is_reference = rhs._is_reference;

	if (_is_reference) {
		_value_ref = rhs._value_ref;
	} else {
		_value = JSONValue(rhs._value, g_allocator);
	}

	return *this;
}

JSON& JSON::operator=(JSON&& rhs)
{
	if (!_is_reference) {
		_value.~JSONValue();
	}

	_is_reference = rhs._is_reference;

	if (_is_reference) {
		_value_ref = rhs._value_ref;
		rhs = JSON();
	} else {
		_value = std::move(rhs._value);
	}

	return *this;
}

JSON& JSON::operator=(const char8_t* value)
{
	getValue().SetString(value, g_allocator);
	return *this;
}

JSON& JSON::operator=(const char* value)
{
	CONVERT_STRING(char8_t, temp_value, value);
	getValue().SetString(temp_value, g_allocator);
	return *this;
}

JSON& JSON::operator=(int32_t value)
{
	getValue().SetInt(value);
	return *this;
}

JSON& JSON::operator=(uint32_t value)
{
	getValue().SetUint(value);
	return *this;
}

JSON& JSON::operator=(int64_t value)
{
	getValue().SetInt64(value);
	return *this;
}

JSON& JSON::operator=(uint64_t value)
{
	getValue().SetUint64(value);
	return *this;
}

JSON& JSON::operator=(double value)
{
	getValue().SetDouble(value);
	return *this;
}

bool JSON::operator==(const JSON& rhs) const
{
	return getValue() == rhs.getValue();
}

bool JSON::operator!=(const JSON& rhs) const
{
	return getValue() != rhs.getValue();
}

NS_END
