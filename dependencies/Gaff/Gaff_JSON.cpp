/************************************************************************************
Copyright (C) 2015 by Nicholas LaCroix

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
#include <cstring>

NS_GAFF

void JSON::SetMemoryFunctions(json_malloc_t alloc_func, json_free_t free_func)
{
	assert(alloc_func && free_func);
	json_set_alloc_funcs(alloc_func, free_func);
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

const char* JSON::getString(void) const
{
	return json_string_value(_value);
}

long long JSON::getInteger(void) const
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

NS_END
