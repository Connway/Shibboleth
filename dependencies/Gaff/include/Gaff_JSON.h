/************************************************************************************
Copyright (C) 2014 by Nicholas LaCroix

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

#include "Gaff_String.h"
#include <jansson.h>

NS_GAFF

class JSON
{
public:
	template <class Callback>
	void forEachInObject(Callback&& callback) const
	{
		assert(_value && isObject());
		const char* key = nullptr;
		json_t* value = nullptr;

		json_object_foreach(_value, key, value)
		{
			if (callback(key, JSON(value, true))) {
				break;
			}
		}
	}

	template <class Callback>
	void forEachInArray(Callback&& callback) const
	{
		assert(_value && isArray());
		json_t* value = nullptr;
		size_t index = 0;

		json_array_foreach(_value, index, value)
		{
			if (callback(index, JSON(value, true))) {
				break;
			}
		}
	}

	template <class Allocator>
	JSON getObject(const AString<Allocator> key)
	{
		return getObject(key.getBuffer());
	}

	template <class Allocator>
	JSON operator[](const AString<Allocator> key)
	{
		return getObject(key.getBuffer());
	}

	template <class Allocator>
	bool parse(const AString<Allocator> input)
	{
		return parse(input.getBuffer());
	}

	static INLINE void SetMemoryFunctions(json_malloc_t alloc_func, json_free_t free_func);
	static INLINE void SetHashSeed(size_t hash_seed);
	static INLINE JSON createArray(void);
	static INLINE JSON createObject(void);
	static INLINE JSON createInteger(json_int_t val);
	static INLINE JSON createReal(double val);
	static INLINE JSON createString(const char* val);
	static INLINE JSON createBoolean(bool val);
	static INLINE JSON createTrue(void);
	static INLINE JSON createFalse(void);
	static INLINE JSON createNull(void);

	JSON(const JSON& json);
	JSON(void);
	~JSON(void);

	INLINE bool parseFile(const char* filename);
	INLINE bool parse(const char* input);
	INLINE bool dumpToFile(const char* filename);
	INLINE char* dump(void);

	INLINE void destroy(void);
	INLINE bool valid(void) const;

	INLINE bool isObject(void) const;
	INLINE bool isArray(void) const;
	INLINE bool isString(void) const;
	INLINE bool isNumber(void) const;
	INLINE bool isInteger(void) const;
	INLINE bool isReal(void) const;
	INLINE bool isBoolean(void) const;
	INLINE bool isTrue(void) const;
	INLINE bool isFalse(void) const;
	INLINE bool isNull(void) const;

	INLINE JSON getObject(const char* key) const;
	INLINE JSON getObject(size_t index) const;
	INLINE const char* getString(void) const;
	INLINE json_int_t getInteger(void) const;
	INLINE double getReal(void) const;
	INLINE double getNumber(void) const;

	INLINE void setObject(const char* key, const JSON& json);
	INLINE void setObject(size_t index, const JSON& json);

	INLINE size_t size(void) const;

	INLINE JSON shallowCopy(void) const;
	INLINE JSON deepCopy(void) const;

	const JSON& operator=(const JSON& rhs);

	const JSON& operator=(const char* value);
	const JSON& operator=(json_int_t value);
	const JSON& operator=(double value);

	INLINE bool operator==(const JSON& rhs) const;
	INLINE bool operator!=(const JSON& rhs) const;
	INLINE JSON operator[](const char* key) const;
	INLINE JSON operator[](size_t index) const;

	INLINE operator bool(void) const { return valid(); }

private:
	json_error_t _error;
	json_t* _value;

	explicit JSON(json_t* json, bool increment_ref_count);
};

NS_END
