/************************************************************************************
Copyright (C) 2021 by Nicholas LaCroix

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

#ifdef _MSC_VER
	#pragma warning(push)
	#pragma warning(disable : 4127)
#endif

#include <rapidjson/filewritestream.h>
#include <rapidjson/filereadstream.h>
#include <rapidjson/prettywriter.h>
#include <rapidjson/error/en.h>
#include <rapidjson/schema.h>

#ifdef _MSC_VER
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

JSON JSON::CreateStringRef(const char* val)
{
	JSONValue value = JSONValue(JSONValue::StringRefType(val));
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

	JSONSchemaDocument sd(schema._value, nullptr, 0, nullptr, &g_allocator);
	JSONSchemaValidator validator(sd);

	if (!getValue().Accept(validator)) {
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

bool JSON::parseFile(const char* filename, const JSON& schema)
{
	using JSONSchemaDocument = rapidjson::GenericSchemaDocument<JSONValue, JSONInternalAllocator>;
	using JSONSchemaValidator = rapidjson::GenericSchemaValidator<JSONSchemaDocument>;

	GAFF_ASSERT(filename && strlen(filename));
	*this = CreateNull();

	FILE* file = nullptr;
	fopen_s(&file, filename, "r");

	if (!file) {
		return false;
	}

	char buffer[2048];

	JSONSchemaDocument sd(schema.getValue(), nullptr, 0, nullptr, &g_allocator);
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

bool JSON::parseFile(const char* filename, const char* schema_input)
{
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
	*this = CreateNull();

	FILE* file = nullptr;
	fopen_s(&file, filename, "r");

	if (!file) {
		return false;
	}

	char buffer[2048];
	rapidjson::FileReadStream stream(file, buffer, 2048);

	JSONDocument document;
	document.ParseStream(stream);

	fclose(file);

	if (document.HasParseError()) {
		_error.Set(document.GetParseError(), document.GetErrorOffset());
		return false;
	}

	JSONValue& value = document;
	_value = std::move(value);
	_is_reference = false;
	return true;
}

bool JSON::parse(const char* input, const JSON& schema)
{
	GAFF_ASSERT(input && strlen(input));
	*this = CreateNull();

	using JSONSchemaDocument = rapidjson::GenericSchemaDocument<JSONValue, JSONInternalAllocator>;
	using JSONSchemaValidator = rapidjson::GenericSchemaValidator<JSONSchemaDocument>;

	JSONSchemaDocument sd(schema.getValue(), nullptr, 0, nullptr, &g_allocator);
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

bool JSON::parse(const char* input, const char* schema_input)
{
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

bool JSON::dumpToFile(const char* filename) const
{
	GAFF_ASSERT(isArray() || isObject());
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

const char* JSON::dump(void)
{
	GAFF_ASSERT(isArray() || isObject());
	JSONStringBuffer buffer;

	rapidjson::PrettyWriter<
		JSONStringBuffer, rapidjson::UTF8<>,
		rapidjson::UTF8<>, JSONInternalAllocator
	> writer(buffer);

	if (WriteJSON(*this, writer)) {
		size_t size = buffer.GetSize();
		char* const str = reinterpret_cast<char*>(g_alloc(size + 1));

		memcpy_s(str, size, buffer.GetString(), size);
		str[size] = 0;

		return str;
	}

	return nullptr;
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

JSON JSON::getObject(const char* key) const
{
	auto it = getValue().FindMember(key);

	if (it == getValue().MemberEnd()) {
		return CreateNull();
	}

	return JSON(const_cast<JSONValue*>(&it->value));
}

JSON JSON::getObject(int32_t index) const
{
	return JSON(const_cast<JSONValue*>(&getValue()[static_cast<rapidjson::SizeType>(index)]));
}

const char* JSON::getKey(char* buffer, size_t buf_size, int32_t index) const
{
	GAFF_ASSERT(isObject() && index < size());
	const char* const key = (getValue().MemberBegin() + index)->name.GetString();
	strncpy(buffer, key, buf_size - 1);
	return buffer;
}

const char* JSON::getKey(int32_t index) const
{
	GAFF_ASSERT(isObject() && index < size());
	return (getValue().MemberBegin() + index)->name.GetString();
}

JSON JSON::getValue(int32_t index) const
{
	GAFF_ASSERT(isObject() && index < size());
	return JSON(const_cast<JSONValue*>(&(getValue().MemberBegin() + index)->value));
}

const char* JSON::getString(char* buffer, size_t buf_size, const char* default_value) const
{
	return (isNull()) ? strncpy(buffer, default_value, buf_size - 1) : strncpy(buffer, getValue().GetString(), buf_size - 1);
}

const char* JSON::getString(const char* default_value) const
{
	return (isNull()) ? default_value : getValue().GetString();
}

int8_t JSON::getInt8(int8_t default_value) const
{
	return (isNull()) ? default_value : getInt8();
}

uint8_t JSON::getUInt8(uint8_t default_value) const
{
	return (isNull()) ? default_value : getUInt8();
}

int16_t JSON::getInt16(int16_t default_value) const
{
	return (isNull()) ? default_value : getInt16();
}

uint16_t JSON::getUInt16(uint16_t default_value) const
{
	return (isNull()) ? default_value : getUInt16();
}

int32_t JSON::getInt32(int32_t default_value) const
{
	return (isNull()) ? default_value : getInt32();
}

uint32_t JSON::getUInt32(uint32_t default_value) const
{
	return (isNull()) ? default_value : getUInt32();
}

int64_t JSON::getInt64(int64_t default_value) const
{
	return (isNull()) ? default_value : getInt64();
}

uint64_t JSON::getUInt64(uint64_t default_value) const
{
	return (isNull()) ? default_value : getUInt64();
}

float JSON::getFloat(float default_value) const
{
	return (isNull()) ? default_value : getFloat();
}

double JSON::getDouble(double default_value) const
{
	return (isNull()) ? default_value : getDouble();
}

double JSON::getNumber(double default_value) const
{
	return (isNull()) ? default_value : getNumber();
}

bool JSON::getBool(bool default_value) const
{
	return (isNull()) ? default_value : getBool();
}

const char* JSON::getString(char* buffer, size_t buf_size) const
{
	return strncpy(buffer, getString(), buf_size - 1);
}

const char* JSON::getString(void) const
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

void JSON::setObject(const char* key, const JSON& json)
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

void JSON::setObject(const char* key, JSON&& json)
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

JSON& JSON::operator=(const char* value)
{
	getValue().SetString(value, g_allocator);
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

JSON JSON::operator[](const char* key) const
{
	return getObject(key);
}

JSON JSON::operator[](int32_t index) const
{
	return getObject(index);
}

NS_END
