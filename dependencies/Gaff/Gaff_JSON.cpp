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
#include <cstring>
#include <cstdio>

#ifdef _WIN32
	#if _MSC_VER < 1900
		#ifndef snprintf
			#define snprintf _snprintf
		#endif
	#endif
#endif


NS_GAFF

class JSON::JSONAllocator
{
public:
	void* alloc(size_t size_bytes)
	{
		return JSON::_alloc(size_bytes);
	}

	void free(void* data)
	{
		JSON::_free(data);
	}
};

struct JSON::ElementInfo
{
	const char* type[16] = { nullptr };
	size_t num_types = 0;
	JSON schema[16];
	size_t num_schemas = 0;
	const char* key = nullptr;
	bool optional = false;
	JSON requires;
	size_t depth = 0;
};



json_malloc_t JSON::_alloc = malloc;
json_free_t JSON::_free = free;

void JSON::SetMemoryFunctions(json_malloc_t alloc_func, json_free_t free_func)
{
	assert(alloc_func && free_func);
	json_set_alloc_funcs(alloc_func, free_func);
	_alloc = alloc_func;
	_free = free_func;
}

/*!
	\brief Sets the seed the system uses to calculate hashes.
	\param hash_seed The hash seed to use.
	\note
		If you are using JSON objects across DLL boundaries, make sure to call this in the DLL as well!
		These hashes have to match, so they must use the same seed!
*/
void JSON::SetHashSeed(size_t hash_seed)
{
	json_object_seed(hash_seed);
}

JSON JSON::createArray(void)
{
	return JSON(json_array(), false);
}

JSON JSON::createObject(void)
{
	return JSON(json_object(), false);
}

JSON JSON::createInteger(json_int_t val)
{
	return JSON(json_integer(val), false);
}

JSON JSON::createReal(double val)
{
	return JSON(json_real(val), false);
}

JSON JSON::createString(const char* val)
{
	return JSON(json_string(val), false);
}

JSON JSON::createBoolean(bool val)
{
	return JSON(json_boolean(val), true);
}

JSON JSON::createTrue(void)
{
	return JSON(json_true(), true);
}

JSON JSON::createFalse(void)
{
	return JSON(json_false(), true);
}

JSON JSON::createNull(void)
{
	return JSON(json_null(), true);
}

JSON::JSON(json_t* json, bool increment_ref_count):
	_value(json)
{
	if (_value && increment_ref_count) {
		json_incref(_value);
	}
}

JSON::JSON(const JSON& json):
	_value(json._value)
{
	json_incref(_value);
}

JSON::JSON(void):
	_value(nullptr)
{
}

JSON::~JSON(void)
{
	destroy();
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
			memcpy(_error.source, schema_file, Min(static_cast<size_t>(JSON_ERROR_SOURCE_LENGTH), strlen(schema_file) + 1));
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
					snprintf(_error.text, JSON_ERROR_TEXT_LENGTH, "Value at index '%zu' is not an object.", index);
					return true;
				}

				JSON name = value["Name"];

				if (!name) {
					snprintf(_error.text, JSON_ERROR_TEXT_LENGTH, "Value at index '%zu' is not an object.", index);
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
		_error = schema_object._error;
		return false;
	}

	return validate(schema_object);
}

bool JSON::parseFile(const char* filename)
{
	assert(!_value && filename && strlen(filename));
	_value = json_load_file(filename, JSON_REJECT_DUPLICATES | JSON_DISABLE_EOF_CHECK, &_error);
	return valid();
}

bool JSON::parse(const char* input)
{
	assert(!_value && input);
	_value = json_loads(input, JSON_REJECT_DUPLICATES | JSON_DISABLE_EOF_CHECK, &_error);
	return valid();
}

bool JSON::dumpToFile(const char* filename)
{
	assert(_value);
	return !json_dump_file(_value, filename, JSON_INDENT(4) | JSON_SORT_KEYS);
}

char* JSON::dump(void)
{
	assert(_value);
	return json_dumps(_value, JSON_INDENT(4) | JSON_SORT_KEYS);
}

void JSON::destroy(void)
{
	if (_value) {
		json_decref(_value);
		_value = nullptr;
	}
}

bool JSON::valid(void) const
{
	return _value != nullptr;
}

bool JSON::isObject(void) const
{
	return json_is_object(_value);
}

bool JSON::isArray(void) const
{
	return json_is_array(_value);
}

bool JSON::isString(void) const
{
	return json_is_string(_value);
}

bool JSON::isNumber(void) const
{
	return json_is_number(_value);
}

bool JSON::isInteger(void) const
{
	return json_is_integer(_value);
}

bool JSON::isReal(void) const
{
	return json_is_real(_value);
}

bool JSON::isBoolean(void) const
{
	return json_is_boolean(_value);
}

bool JSON::isTrue(void) const
{
	return json_is_true(_value);
}

bool JSON::isFalse(void) const
{
	return json_is_false(_value);
}

bool JSON::isNull(void) const
{
	return json_is_null(_value);
}

/*!
	\brief If this is a JSON object, get the value associated with \a key.
	\param key The key whose value to retrieve.
	\return Returns the value found at \a key.
*/
JSON JSON::getObject(const char* key) const
{
	assert(isObject());
	return JSON(json_object_get(_value, key), true);
}

/*!
	\brief If this is a JSON array, get the value at \a index.
	\param index The index in the array to retrieve.
	\return Returns the value found at \a index.
*/
JSON JSON::getObject(size_t index) const
{
	assert(isArray() && index < size());
	return JSON(json_array_get(_value, index), true);
}

const char* JSON::getString(const char* default_value) const
{
	return (_value) ? json_string_value(_value) : default_value;
}

json_int_t JSON::getInteger(json_int_t default_value) const
{
	return (_value) ? json_integer_value(_value) : default_value;
}

double JSON::getReal(double default_value) const
{
	return (_value) ? json_real_value(_value) : default_value;
}

double JSON::getNumber(double default_value) const
{
	return (_value) ? json_number_value(_value) : default_value;
}

const char* JSON::getString(void) const
{
	return json_string_value(_value);
}

json_int_t JSON::getInteger(void) const
{
	return json_integer_value(_value);
}

double JSON::getReal(void) const
{
	return json_real_value(_value);
}

double JSON::getNumber(void) const
{
	return json_number_value(_value);
}

/*!
	\brief If this is a JSON object, set the value associated with \a key to \a json.
	\param index The index in the array to set.
	\param json The value associated with \a key.
*/
void JSON::setObject(const char* key, const JSON& json)
{
	assert(json.valid() && isObject());
	json_object_set(_value, key, json._value);
}

/*!
	\brief If this is a JSON array, set the value at \a index to \a json.
	\param index The key whose value we are going to set.
	\param json The value to set at \a index.
*/
void JSON::setObject(size_t index, const JSON& json)
{
	assert(json.valid() && isArray());
	json_array_set(_value, index, json._value);
}

/*!
	\brief
		Gets either the number of key/value pairs or the number of elements,
		depending on if it is a JSON object or a JSON array.
*/
size_t JSON::size(void) const
{
	assert(_value && (isArray() || isObject()));
	return (isObject()) ? json_object_size(_value) : json_array_size(_value);
}

JSON JSON::shallowCopy(void) const
{
	assert(_value);
	return JSON(json_copy(_value), false);
}

JSON JSON::deepCopy(void) const
{
	assert(_value);
	return JSON(json_deep_copy(_value), false);
}

int JSON::getErrorLine(void) const
{
	return _error.line;
}

int JSON::getErrorColumn(void) const
{
	return _error.column;
}

int JSON::getErrorPosition(void) const
{
	return _error.position;
}

const char* JSON::getErrorSource(void) const
{
	return _error.source;
}

const char* JSON::getErrorText(void) const
{
	return _error.text;
}

const JSON& JSON::operator=(const JSON& rhs)
{
	destroy();
	_value = rhs._value;
	json_incref(_value);
	return *this;
}

const JSON& JSON::operator=(const char* value)
{
	assert(_value && isString());
	json_string_set(_value, value);
	return *this;
}

const JSON& JSON::operator=(json_int_t value)
{
	assert(_value && isInteger());
	json_integer_set(_value, value);
	return *this;
}

const JSON& JSON::operator=(double value)
{
	assert(_value && isReal());
	json_real_set(_value, value);
	return *this;
}

bool JSON::operator==(const JSON& rhs) const
{
	return json_equal(_value, rhs._value) == 1;
}

bool JSON::operator!=(const JSON& rhs) const
{
	return !(*this == rhs);
}

/*!
	\brief This is the same as calling getObject(\a key).
*/
JSON JSON::operator[](const char* key) const
{
	return getObject(key);
}

/*!
	\brief This is the same as calling getObject(\a index).
*/
JSON JSON::operator[](size_t index) const
{
	return getObject(index);
}

void JSON::ExtractElementInfoHelper(
	ElementInfo& info, size_t type_index, size_t& schema_index,
	const JSON& type, const JSON& schema, const SchemaMap& schema_map,
	bool is_object, bool is_array)
{
	info.type[type_index] = type.getString();

	if (is_array || is_object) {
		assert((schema.isArray() && schema_index < schema.size()) || schema_index == 0);
		JSON s = (schema.isArray()) ? schema[schema_index] : schema;

		if (s.isString()) {
			assert(schema_map.hasElementWithKey(s.getString()));
			s = schema_map[s.getString()];
		}

		info.schema[schema_index] = s;
		++schema_index;
	}
}

JSON::ElementInfo JSON::ExtractElementInfo(JSON element, const SchemaMap& schema_map)
{
	assert(element.isString() || element.isObject());
	ElementInfo info;

	if (element.isString()) {
		element = schema_map[element.getString()];
	}

	JSON type = element["Type"];
	JSON schema = element["Schema"];

	assert(type.isString() || type.isArray());
	assert(!schema || schema.isString() || schema.isArray() || schema.isObject());
	assert(element["Optional"].isBoolean());
	assert(!element["Requires"] || element["Requires"].isString() || element["Requires"].isArray());

	bool is_object = !strcmp(type.getString(), "Object");
	bool is_array = !strcmp(type.getString(), "Array");

	assert(!(is_array || is_object) || schema);

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
		parse_funcs["Real"] = &JSON::isRealSchema;
		parse_funcs["Boolean"] = &JSON::isBooleanSchema;
	}

	return !schema.forEachInObject([&](const char* key, const JSON& value) -> bool
	{
		ElementInfo info = ExtractElementInfo(value, schema_map);
		JSON element = getObject(key);
		info.key = key;

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
	if (!info.requires) {
		return true;
	}

	auto check_lambda = [&](size_t, const JSON& value) -> bool
	{
		assert(value.isString());

		if (getObject(value.getString()).isNull()) {
			snprintf(_error.text, JSON_ERROR_TEXT_LENGTH, "Element '%s' requires element '%s'.", info.key, value.getString());
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
	if (!element && info.optional) {
		return true;
	} else if (!element && !info.optional) {
		snprintf(_error.text, JSON_ERROR_TEXT_LENGTH, "Element '%s' is non-optional.", info.key);
		return false;
	}

	if (!element.isObject()) {
		snprintf(_error.text, JSON_ERROR_TEXT_LENGTH, "Element '%s' is not an object.", info.key);
		return false;
	}

	size_t index = s_index;
	++s_index;

	if (!element.validateSchema(info.schema[index], schema_map)) {
		_error = element._error;
		return false;
	}

	return true;
}

bool JSON::isArraySchema(const JSON& element, const ElementInfo& info, const SchemaMap& schema_map, size_t& s_index) const
{
	if (!element && info.optional) {
		return true;
	} else if (!element && !info.optional) {
		snprintf(_error.text, JSON_ERROR_TEXT_LENGTH, "Element '%s' is non-optional.", info.key);
		return false;
	}

	if (!element.isArray()) {
		snprintf(_error.text, JSON_ERROR_TEXT_LENGTH, "Element '%s' is not an array.", info.key);
		return false;
	}

	JSON schema = info.schema[s_index];
	++s_index;

	JSON s = schema["Schema"];

	assert(!s || s.isString() || s.isObject());
	assert(schema["Type"].isString());

	const char* type = schema["Type"].getString();

	if (s.isString()) {
		assert(schema_map.hasElementWithKey(s.getString()));
		s = schema_map[s.getString()];
	}

	bool is_string = !strcmp(type, "String");
	bool is_number = !strcmp(type, "Number");
	bool is_integer = !strcmp(type, "Integer");
	bool is_real = !strcmp(type, "Real");
	bool is_boolean = !strcmp(type, "Boolean");
	bool is_object = !strcmp(type, "Object");
	bool is_array = !strcmp(type, "Array");

	assert(!(is_object || is_array) || s);

	return !element.forEachInArray([&](size_t index, const JSON& value) -> bool
	{
		if (is_string && !value.isString()) {
			snprintf(_error.text, JSON_ERROR_TEXT_LENGTH, "Element '%s' index '%zu' depth '%zu' is not a string.", info.key, index, info.depth);
			return true;
		} else if (is_number && !value.isNumber()) {
			snprintf(_error.text, JSON_ERROR_TEXT_LENGTH, "Element '%s' index '%zu' depth '%zu' is not a number.", info.key, index, info.depth);
			return true;
		} else if (is_integer && !value.isInteger()) {
			snprintf(_error.text, JSON_ERROR_TEXT_LENGTH, "Element '%s' index '%zu' depth '%zu' is not an integer.", info.key, index, info.depth);
			return true;
		} else if (is_real && !value.isReal()) {
			snprintf(_error.text, JSON_ERROR_TEXT_LENGTH, "Element '%s' index '%zu' depth '%zu' is not a real.", info.key, index, info.depth);
			return true;
		} else if (is_boolean && !value.isBoolean()) {
			snprintf(_error.text, JSON_ERROR_TEXT_LENGTH, "Element '%s' index '%zu' depth '%zu' is not a boolean.", info.key, index, info.depth);
			return true;

		} else if (is_object) {
			if (!value.isObject()) {
				snprintf(_error.text, JSON_ERROR_TEXT_LENGTH, "Element '%s' index '%zu' depth '%zu' is not an object.", info.key, index, info.depth);
				return true;
			}

			if (!value.validateSchema(s, schema_map)) {
				_error = value._error;
				return true;
			}

		} else if (is_array) {
			if (!value.isArray()) {
				snprintf(_error.text, JSON_ERROR_TEXT_LENGTH, "Element '%s' index '%zu' depth '%zu' is not an array.", info.key, index, info.depth);
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
	if (!element && info.optional) {
		return true;
	} else if (!element && !info.optional) {
		snprintf(_error.text, JSON_ERROR_TEXT_LENGTH, "Element '%s' is non-optional.", info.key);
		return false;
	}

	if (!element.isString()) {
		snprintf(_error.text, JSON_ERROR_TEXT_LENGTH, "Element '%s' is not a string.", info.key);
		return false;
	}

	return true;
}

bool JSON::isNumberSchema(const JSON& element, const ElementInfo& info, const SchemaMap&, size_t&) const
{
	if (!element && info.optional) {
		return true;
	} else if (!element && !info.optional) {
		snprintf(_error.text, JSON_ERROR_TEXT_LENGTH, "Element '%s' is non-optional.", info.key);
		return false;
	}

	if (!element.isNumber()) {
		snprintf(_error.text, JSON_ERROR_TEXT_LENGTH, "Element '%s' is not a number.", info.key);
		return false;
	}

	return true;
}

bool JSON::isIntegerSchema(const JSON& element, const ElementInfo& info, const SchemaMap&, size_t&) const
{
	if (!element && info.optional) {
		return true;
	} else if (!element && !info.optional) {
		snprintf(_error.text, JSON_ERROR_TEXT_LENGTH, "Element '%s' is non-optional.", info.key);
		return false;
	}

	if (!element.isInteger()) {
		snprintf(_error.text, JSON_ERROR_TEXT_LENGTH, "Element '%s' is not an integer.", info.key);
		return false;
	}

	return true;
}

bool JSON::isRealSchema(const JSON& element, const ElementInfo& info, const SchemaMap&, size_t&) const
{
	if (!element && info.optional) {
		return true;
	} else if (!element && !info.optional) {
		snprintf(_error.text, JSON_ERROR_TEXT_LENGTH, "Element '%s' is non-optional.", info.key);
		return false;
	}

	if (!element.isReal()) {
		snprintf(_error.text, JSON_ERROR_TEXT_LENGTH, "Element '%s' is not a real.", info.key);
		return false;
	}

	return true;
}

bool JSON::isBooleanSchema(const JSON& element, const ElementInfo& info, const SchemaMap&, size_t&) const
{
	if (!element && info.optional) {
		return true;
	} else if (!element && !info.optional) {
		snprintf(_error.text, JSON_ERROR_TEXT_LENGTH, "Element '%s' is non-optional.", info.key);
		return false;
	}

	if (!element.isBoolean()) {
		snprintf(_error.text, JSON_ERROR_TEXT_LENGTH, "Element '%s' is not a boolean.", info.key);
		return false;
	}

	return true;
}

NS_END
