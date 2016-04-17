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

#include "Gaff_JSON.h"
#include "Gaff_File.h"
#include <rapidjson/filewritestream.h>
#include <rapidjson/filereadstream.h>
#include <rapidjson/prettywriter.h>
#include <rapidjson/error/en.h>

NS_GAFF

class JSON::JSONAllocator
{
public:
	static const bool kNeedFree = true;

	void* Malloc(size_t size)
	{
		return (size) ? _alloc(size) : nullptr;
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
			_free(ptr);
			return nullptr;
		}


		void* new_ptr = _alloc(new_size);
		memcpy_s(new_ptr, new_size, ptr, original_size);
		_free(ptr);

		return new_ptr;
	}

	static void Free(void* ptr)
	{
		_free(ptr);
	}
};

class JSON::JSONAllocatorOld
{
public:
	void* alloc(size_t size_bytes, const char* /*file*/, int /*line*/)
	{
		return _alloc(size_bytes);
	}

	void free(void* data)
	{
		_free(data);
	}
};

struct JSON::ElementInfo
{
	char type[16][16];
	size_t num_types = 0;
	JSON schema[16];
	size_t num_schemas = 0;
	char key[64] = { 0 };
	bool optional = false;
	JSON requires;
	size_t depth = 0;
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

	} else if (json.isInt()) {
		success = writer.Int(json.getInt());

	} else if (json.isUInt()) {
		success = writer.Uint(json.getUInt());

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

JSON::JSONAlloc JSON::_alloc = malloc;
JSON::JSONFree JSON::_free = free;

void JSON::SetMemoryFunctions(JSONAlloc alloc_func, JSONFree free_func)
{
	GAFF_ASSERT(alloc_func && free_func);
	_alloc = alloc_func;
	_free = free_func;
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

JSON JSON::CreateInt(int val)
{
	JSONValue value = JSONValue(val);
	return JSON(std::move(value));
}

JSON JSON::CreateUInt(unsigned int val)
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
	JSONAllocator allocator;
	JSONValue value = JSONValue(val, allocator);
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

JSON::JSON(const JSONValue& json)
{
	JSONAllocator allocator;
	_value = JSONValue(json, allocator);
}

JSON::JSON(const JSON& json)
{
	*this = json;
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
	File file;

	if (!file.open(schema_file, File::READ_BINARY)) {
		return false;
	}

	char* input = reinterpret_cast<char*>(_alloc(sizeof(char) * file.getFileSize()));
	bool ret_val = false;
	
	if (input) {
		if (file.readEntireFile(input)) {
			//memcpy(_error.source, schema_file, Min(static_cast<size_t>(JSON_ERROR_SOURCE_LENGTH), strlen(schema_file) + 1));
			ret_val = validate(input);
		}

		_free(input);
	}

	return ret_val;
}

// Does not support either-or types for Arrays. Unless a use-case comes up,
// I have no plans to support it.
bool JSON::validate(const JSON& schema_object) const
{
	SchemaMap schema_map;
	JSON schema = schema_object;

	if (schema_object.isArray()) {
		schema_map.reserve(schema_object.size() - 1);

		bool error = schema_object.forEachInArray([&](size_t index, const JSON& value) -> bool
		{
			if (index != schema.size() - 1) {
				if (!value.isObject()) {
					//snprintf(_error.text, JSON_ERROR_TEXT_LENGTH, "Value at index '%zu' is not an object.", index);
					return true;
				}

				JSON name = value["Name"];

				if (!name.isString()) {
					//snprintf(_error.text, JSON_ERROR_TEXT_LENGTH, "Value at index '%zu' is not an object.", index);
					return true;
				}

				schema_map[name.getString()] = value["Schema"];
			}

			return false;
		});

		if (error) {
			return false;
		}

		schema = schema_object[schema_object.size() - 1];
	}

	return validateSchema(schema, schema_map);
}

bool JSON::validate(const char* input) const
{
	JSON schema_object;

	if (!schema_object.parse(input)) {
		//_error = schema_object._error;
		return false;
	}

	return validate(schema_object);
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
		rapidjson::UTF8<>, JSONAllocator
	> writer(stream);

	bool success = WriteJSON(*this, writer);
	fclose(file);

	return success;
}

char* JSON::dump(void)
{
	GAFF_ASSERT(_value.IsArray() || _value.IsObject());
	//return json_dumps(_value, JSON_INDENT(4) | JSON_SORT_KEYS);
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

bool JSON::isInt(void) const
{
	return _value.IsInt();
}

bool JSON::isUInt(void) const
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

JSON JSON::getObject(size_t index) const
{
	return JSON(_value[static_cast<rapidjson::SizeType>(index)]);
}

const char* JSON::getString(const char* default_value) const
{
	return (_value.IsNull()) ? default_value : _value.GetString();
}

int JSON::getInt(int default_value) const
{
	return (_value.IsNull()) ? default_value : _value.GetInt();
}

unsigned int JSON::getUInt(unsigned int default_value) const
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

const char* JSON::getString(void) const
{
	return _value.GetString();
}

int JSON::getInt(void) const
{
	return _value.GetInt();
}

unsigned int JSON::getUInt(void) const
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
	JSONAllocator allocator;
	JSONValue value(json._value, allocator);

	auto it = _value.FindMember(key);

	if (it != _value.MemberEnd()) {
		it->value = std::move(value);
	} else {
		_value.AddMember(JSONValue(key, allocator), std::move(value), allocator);
	}
}

void JSON::setObject(const char* key, JSON&& json)
{
	GAFF_ASSERT(_value.IsObject());
	auto it = _value.FindMember(key);

	if (it != _value.MemberEnd()) {
		it->value = std::move(json._value);
	} else {
		JSONAllocator allocator;
		_value.AddMember(JSONValue(key, allocator), std::move(json._value), allocator);
	}
}

void JSON::setObject(size_t index, const JSON& json)
{
	GAFF_ASSERT(_value.IsArray() && index < _value.Size());
	JSONAllocator allocator;
	JSONValue value(json._value, allocator);

	_value[static_cast<rapidjson::SizeType>(index)] = std::move(value);
}

void JSON::setObject(size_t index, JSON&& json)
{
	GAFF_ASSERT(_value.IsArray() && index < _value.Size());
	_value[static_cast<rapidjson::SizeType>(index)] = std::move(json._value);
}

void JSON::push(const JSON& json)
{
	GAFF_ASSERT(_value.IsArray());
	JSONAllocator allocator;
	JSONValue value(json._value, allocator);

	_value.PushBack(std::move(value), allocator);
}

void JSON::push(JSON&& json)
{
	GAFF_ASSERT(_value.IsArray());
	JSONAllocator allocator;
	_value.PushBack(std::move(json._value), allocator);
}

size_t JSON::size(void) const
{
	GAFF_ASSERT(isArray() || isObject() || isString());

	size_t size = 0;

	if (isArray()) {
		size = _value.Size();
	} else if (isObject()) {
		size = _value.MemberCount();
	} else {
		size = _value.GetStringLength();
	}

	return size;
}

const char* JSON::getErrorText(void) const
{
	return _error.IsError() ? rapidjson::GetParseError_En(_error.Code()) : nullptr;
}

const JSON& JSON::operator=(const JSON& rhs)
{
	JSONAllocator allocator;
	_value = JSONValue(rhs._value, allocator);
	return *this;
}

const JSON& JSON::operator=(JSON&& rhs)
{
	_value = std::move(rhs._value);
	return *this;
}

const JSON& JSON::operator=(const char* value)
{
	JSONAllocator allocator;
	_value.SetString(value, allocator);
	return *this;
}

const JSON& JSON::operator=(int value)
{
	_value.SetInt(value);
	return *this;
}

const JSON& JSON::operator=(unsigned int value)
{
	_value.SetUint(value);
	return *this;
}

const JSON& JSON::operator=(int64_t value)
{
	_value.SetInt64(value);
	return *this;
}

const JSON& JSON::operator=(uint64_t value)
{
	_value.SetUint64(value);
	return *this;
}

const JSON& JSON::operator=(double value)
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

JSON JSON::operator[](size_t index) const
{
	return getObject(index);
}

void JSON::ExtractElementInfoHelper(
	ElementInfo& info, size_t type_index, size_t& schema_index,
	const JSON& type, const JSON& schema, const SchemaMap& schema_map,
	bool is_object, bool is_array)
{
	strncpy_s(info.type[type_index], type.getString(), 16);

	if (is_array || is_object) {
		GAFF_ASSERT((schema.isArray() && schema_index < schema.size()) || schema_index == 0);
		JSON s = (schema.isArray()) ? schema[schema_index] : schema;

		if (s.isString()) {
			GAFF_ASSERT(schema_map.hasElementWithKey(s.getString()));
			s = schema_map[s.getString()];
		}

		info.schema[schema_index] = s;
		++schema_index;
	}
}

JSON::ElementInfo JSON::ExtractElementInfo(JSON element, const SchemaMap& schema_map)
{
	GAFF_ASSERT(element.isString() || element.isObject());
	ElementInfo info;

	if (element.isString()) {
		element = schema_map[element.getString()];
	}

	JSON type = element["Type"];
	JSON schema = element["Schema"];

	GAFF_ASSERT(type.isString() || type.isArray());
	GAFF_ASSERT(schema.isNull() || schema.isString() || schema.isArray() || schema.isObject());
	GAFF_ASSERT(element["Optional"].isBool());
	GAFF_ASSERT(element["Requires"].isNull() || element["Requires"].isString() || element["Requires"].isArray());

	bool is_object = !strcmp(type.getString(), "Object");
	bool is_array = !strcmp(type.getString(), "Array");

	GAFF_ASSERT(!(is_array || is_object) || !schema.isNull());

	info.optional = element["Optional"].isTrue();
	info.requires = element["Requires"];

	if (type.isArray()) {
		size_t j = 0;

		info.num_types = type.size();

		for (size_t i = 0; i < info.num_types; ++i) {
			ExtractElementInfoHelper(info, i, j, type, schema, schema_map, is_object, is_array);
		}

		info.num_schemas = j;

	} else {
		size_t i = 0;

		ExtractElementInfoHelper(info, 0, i, type, schema, schema_map, is_object, is_array);

		info.num_types = 1;
		info.num_schemas = i;
	}

	return info;
}

bool JSON::validateSchema(const JSON& schema, const SchemaMap& schema_map) const
{
	static ElementParseMap parse_funcs;

	if (parse_funcs.empty()) {
		parse_funcs.reserve(7);
		parse_funcs["Object"] = &JSON::isObjectSchema;
		parse_funcs["Array"] = &JSON::isArraySchema;
		parse_funcs["String"] = &JSON::isStringSchema;
		parse_funcs["Number"] = &JSON::isNumberSchema;
		parse_funcs["Integer"] = &JSON::isIntegerSchema;
		parse_funcs["Double"] = &JSON::isDoubleSchema;
		parse_funcs["Bool"] = &JSON::isBoolSchema;
	}

	return !schema.forEachInObject([&](const char* key, const JSON& value) -> bool
	{
		ElementInfo info = ExtractElementInfo(value, schema_map);
		JSON element = getObject(key);
		strncpy_s(info.key, key, 64);

		if (!checkRequirements(info)) {
			return false;
		}

		for (size_t i = 0, j = 0; i < info.num_types; ++i) {
			if ((this->*parse_funcs[info.type[i]])(element, info, schema_map, j)) {
				return false;
			}
		}

		return true;
	});
}

bool JSON::checkRequirements(const ElementInfo& info) const
{
	if (info.requires.isNull()) {
		return true;
	}

	auto check_lambda = [&](size_t, const JSON& value) -> bool
	{
		GAFF_ASSERT(value.isString());

		if (getObject(value.getString()).isNull()) {
			//snprintf(_error.text, JSON_ERROR_TEXT_LENGTH, "Element '%s' requires element '%s'.", info.key, value.getString());
			return true;
		}

		return false;
	};

	if (info.requires.isString()) {
		return !check_lambda(0, info.requires);
	}

	return !info.requires.forEachInArray(check_lambda);
}

bool JSON::isObjectSchema(const JSON& element, const ElementInfo& info, const SchemaMap& schema_map, size_t& s_index) const
{
	if (element.isNull() && info.optional) {
		return true;

	} else if (element.isNull() && !info.optional) {
		//snprintf(_error.text, JSON_ERROR_TEXT_LENGTH, "Element '%s' is non-optional.", info.key);
		return false;
	}

	if (!element.isObject()) {
		//snprintf(_error.text, JSON_ERROR_TEXT_LENGTH, "Element '%s' is not an object.", info.key);
		return false;
	}

	size_t index = s_index;
	++s_index;

	if (!element.validateSchema(info.schema[index], schema_map)) {
		//_error = element._error;
		return false;
	}

	return true;
}

bool JSON::isArraySchema(const JSON& element, const ElementInfo& info, const SchemaMap& schema_map, size_t& s_index) const
{
	if (element.isNull() && info.optional) {
		return true;

	} else if (element.isNull() && !info.optional) {
		//snprintf(_error.text, JSON_ERROR_TEXT_LENGTH, "Element '%s' is non-optional.", info.key);
		return false;
	}

	if (!element.isArray()) {
		//snprintf(_error.text, JSON_ERROR_TEXT_LENGTH, "Element '%s' is not an array.", info.key);
		return false;
	}

	JSON schema = info.schema[s_index];
	++s_index;

	JSON s = schema["Schema"];

	GAFF_ASSERT(s.isNull() || s.isString() || s.isObject());
	GAFF_ASSERT(schema["Type"].isString());

	const char* type = schema["Type"].getString();

	if (s.isString()) {
		GAFF_ASSERT(schema_map.hasElementWithKey(s.getString()));
		s = schema_map[s.getString()];
	}

	bool is_string = !strcmp(type, "String");
	bool is_number = !strcmp(type, "Number");
	bool is_integer = !strcmp(type, "Integer");
	bool is_double = !strcmp(type, "Double");
	bool is_bool = !strcmp(type, "Bool");
	bool is_object = !strcmp(type, "Object");
	bool is_array = !strcmp(type, "Array");

	GAFF_ASSERT(!(is_object || is_array) || !s.isNull());

	return !element.forEachInArray([&](size_t /*index*/, const JSON& value) -> bool
	{
		if (is_string && !value.isString()) {
			//snprintf(_error.text, JSON_ERROR_TEXT_LENGTH, "Element '%s' index '%zu' depth '%zu' is not a string.", info.key, index, info.depth);
			return true;
		} else if (is_number && !value.isNumber()) {
			//snprintf(_error.text, JSON_ERROR_TEXT_LENGTH, "Element '%s' index '%zu' depth '%zu' is not a number.", info.key, index, info.depth);
			return true;
		} else if (is_integer && (!value.isNumber() || value.isDouble())) {
			//snprintf(_error.text, JSON_ERROR_TEXT_LENGTH, "Element '%s' index '%zu' depth '%zu' is not an integer.", info.key, index, info.depth);
			return true;
		} else if (is_double && !value.isDouble()) {
			//snprintf(_error.text, JSON_ERROR_TEXT_LENGTH, "Element '%s' index '%zu' depth '%zu' is not a double.", info.key, index, info.depth);
			return true;
		} else if (is_bool && !value.isBool()) {
			//snprintf(_error.text, JSON_ERROR_TEXT_LENGTH, "Element '%s' index '%zu' depth '%zu' is not a bool.", info.key, index, info.depth);
			return true;

		} else if (is_object) {
			if (!value.isObject()) {
				//snprintf(_error.text, JSON_ERROR_TEXT_LENGTH, "Element '%s' index '%zu' depth '%zu' is not an object.", info.key, index, info.depth);
				return true;
			}

			if (!value.validateSchema(s, schema_map)) {
				//_error = value._error;
				return true;
			}

		} else if (is_array) {
			if (!value.isArray()) {
				//snprintf(_error.text, JSON_ERROR_TEXT_LENGTH, "Element '%s' index '%zu' depth '%zu' is not an array.", info.key, index, info.depth);
				return true;
			}

			ElementInfo arr_info = ExtractElementInfo(s, schema_map);
			arr_info.depth = info.depth + 1;
			size_t temp = 0;

			return !isArraySchema(value, arr_info, schema_map, temp);
		}

		return false;
	});
}

bool JSON::isStringSchema(const JSON& element, const ElementInfo& info, const SchemaMap&, size_t&) const
{
	if (element.isNull() && info.optional) {
		return true;

	} else if (element.isNull() && !info.optional) {
		//snprintf(_error.text, JSON_ERROR_TEXT_LENGTH, "Element '%s' is non-optional.", info.key);
		return false;
	}

	if (!element.isString()) {
		//snprintf(_error.text, JSON_ERROR_TEXT_LENGTH, "Element '%s' is not a string.", info.key);
		return false;
	}

	return true;
}

bool JSON::isNumberSchema(const JSON& element, const ElementInfo& info, const SchemaMap&, size_t&) const
{
	if (element.isNull() && info.optional) {
		return true;

	} else if (element.isNull() && !info.optional) {
		//snprintf(_error.text, JSON_ERROR_TEXT_LENGTH, "Element '%s' is non-optional.", info.key);
		return false;
	}

	if (!element.isNumber()) {
		//snprintf(_error.text, JSON_ERROR_TEXT_LENGTH, "Element '%s' is not a number.", info.key);
		return false;
	}

	return true;
}

bool JSON::isIntegerSchema(const JSON& element, const ElementInfo& info, const SchemaMap&, size_t&) const
{
	if (element.isNull() && info.optional) {
		return true;

	} else if (element.isNull() && !info.optional) {
		//snprintf(_error.text, JSON_ERROR_TEXT_LENGTH, "Element '%s' is non-optional.", info.key);
		return false;
	}

	if (!element.isNumber() || element.isDouble()) {
		//snprintf(_error.text, JSON_ERROR_TEXT_LENGTH, "Element '%s' is not an integer.", info.key);
		return false;
	}

	return true;
}

bool JSON::isDoubleSchema(const JSON& element, const ElementInfo& info, const SchemaMap&, size_t&) const
{
	if (element.isNull() && info.optional) {
		return true;

	} else if (element.isNull() && !info.optional) {
		//snprintf(_error.text, JSON_ERROR_TEXT_LENGTH, "Element '%s' is non-optional.", info.key);
		return false;
	}

	if (!element.isDouble()) {
		//snprintf(_error.text, JSON_ERROR_TEXT_LENGTH, "Element '%s' is not a double.", info.key);
		return false;
	}

	return true;
}

bool JSON::isBoolSchema(const JSON& element, const ElementInfo& info, const SchemaMap&, size_t&) const
{
	if (element.isNull() && info.optional) {
		return true;

	} else if (element.isNull() && !info.optional) {
		//snprintf(_error.text, JSON_ERROR_TEXT_LENGTH, "Element '%s' is non-optional.", info.key);
		return false;
	}

	if (!element.isBool()) {
		//snprintf(_error.text, JSON_ERROR_TEXT_LENGTH, "Element '%s' is not a bool.", info.key);
		return false;
	}

	return true;
}

NS_END
