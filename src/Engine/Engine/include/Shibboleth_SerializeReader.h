/************************************************************************************
Copyright (C) 2022 by Nicholas LaCroix

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

#include "Shibboleth_SerializeInterfaces.h"
#include "Shibboleth_ProxyAllocator.h"
#include "Shibboleth_Vector.h"
#include "Shibboleth_String.h"
#include <Gaff_Assert.h>

NS_SHIBBOLETH

template <class Node>
class SerializeReader : public ISerializeReader
{
public:
	SerializeReader(const Node& node, const ProxyAllocator& allocator = ProxyAllocator()):
		_stack(1, node, allocator)
	{
	}

	const Node& getRootNode(void) const { return _stack.front(); }

	bool isObject(void) const override { return _stack.back().isObject(); }
	bool isArray(void) const override { return _stack.back().isArray(); }
	bool isString(void) const override { return _stack.back().isString(); }
	bool isNumber(void) const override { return _stack.back().isNumber(); }
	bool isInt8(void) const override { return _stack.back().isInt8(); }
	bool isUInt8(void) const override { return _stack.back().isUInt8(); }
	bool isInt16(void) const override { return _stack.back().isInt16(); }
	bool isUInt16(void) const override { return _stack.back().isUInt16(); }
	bool isInt32(void) const override { return _stack.back().isInt32(); }
	bool isUInt32(void) const override { return _stack.back().isUInt32(); }
	bool isInt64(void) const override { return _stack.back().isInt64(); }
	bool isUInt64(void) const override { return _stack.back().isUInt64(); }
	bool isFloat(void) const override { return _stack.back().isFloat(); }
	bool isDouble(void) const override { return _stack.back().isDouble(); }
	bool isBool(void) const override { return _stack.back().isBool(); }
	bool isTrue(void) const override { return _stack.back().isTrue(); }
	bool isFalse(void) const override { return _stack.back().isFalse(); }
	bool isNull(void) const override { return _stack.back().isNull(); }

	void enterElement(const char8_t* key) const override
	{
		Node node = _stack.back().getObject(key);
		_stack.push_back(node);
	}

	void enterElement(const char* key) const override
	{
		Node node = _stack.back().getObject(key);
		_stack.push_back(node);
	}

	void enterElement(int32_t index) const override
	{
		Node node = _stack.back().getObject(index);
		_stack.push_back(node);
	}

	void exitElement(void) const override
	{
		GAFF_ASSERT(_stack.size() > 1);
		_stack.pop_back();
	}

	ScopeGuard enterElementGuard(const char8_t* key) const override { return ScopeGuard(*this, key); }
	ScopeGuard enterElementGuard(const char* key) const override { CONVERT_STRING(char8_t, temp_key, key); return ScopeGuard(*this, temp_key); }
	ScopeGuard enterElementGuard(int32_t index) const override { return ScopeGuard(*this, index); }

	const char8_t* getKey(char8_t* buffer, size_t buf_size, int32_t index) const override { return _stack.back().getKey(buffer, buf_size, index); }
	const char8_t* getKey(int32_t index) const override { return _stack.back().getKey(index); }

	void freeString(const char8_t* str) const override { if (isString()) _stack.back().freeString(str); }
	int32_t size(void) const override { return _stack.back().size(); }

	bool exists(const char8_t* key) const override { return !_stack.back().getObject(key).isNull(); };
	bool exists(const char* key) const override { return !_stack.back().getObject(key).isNull(); };

	const char8_t* readString(char8_t* buffer, size_t buf_size, const char8_t* default_value) const override { return _stack.back().getString(buffer, buf_size, default_value); }
	const char8_t* readString(const char8_t* default_value) const override { return _stack.back().getString(default_value); }
	int8_t readInt8(int8_t default_value) const override { return _stack.back().getInt8(default_value); }
	uint8_t readUInt8(uint8_t default_value) const override { return _stack.back().getUInt8(default_value); }
	int16_t readInt16(int16_t default_value) const override { return _stack.back().getInt16(default_value); }
	uint16_t readUInt16(uint16_t default_value) const override { return _stack.back().getUInt16(default_value); }
	int32_t readInt32(int32_t default_value) const override { return _stack.back().getInt32(default_value); }
	uint32_t readUInt32(uint32_t default_value) const override { return _stack.back().getUInt32(default_value); }
	int64_t readInt64(int64_t default_value) const override { return _stack.back().getInt64(default_value); }
	uint64_t readUInt64(uint64_t default_value) const override { return _stack.back().getUInt64(default_value); }
	float readFloat(float default_value) const override { return _stack.back().getFloat(default_value); }
	double readDouble(double default_value) const override { return _stack.back().getDouble(default_value); }
	double readNumber(double default_value) const override { return _stack.back().getNumber(default_value); }
	bool readBool(bool default_value) const override { return _stack.back().getBool(default_value); }

	const char8_t* readString(char8_t* buffer, size_t buf_size) const override { return _stack.back().getString(buffer, buf_size); }
	const char8_t* readString(void) const override { return _stack.back().getString(); }
	int8_t readInt8(void) const override { return _stack.back().getInt8(); }
	uint8_t readUInt8(void) const override { return _stack.back().getUInt8(); }
	int16_t readInt16(void) const override { return _stack.back().getInt16(); }
	uint16_t readUInt16(void) const override { return _stack.back().getUInt16(); }
	int32_t readInt32(void) const override { return _stack.back().getInt32(); }
	uint32_t readUInt32(void) const override { return _stack.back().getUInt32(); }
	int64_t readInt64(void) const override { return _stack.back().getInt64(); }
	uint64_t readUInt64(void) const override { return _stack.back().getUInt64(); }
	float readFloat(void) const override { return _stack.back().getFloat(); }
	double readDouble(void) const override { return _stack.back().getDouble(); }
	double readNumber(void) const override { return _stack.back().getNumber(); }
	bool readBool(void) const override { return _stack.back().getBool(); }

	//const char8_t* readString(const char8_t* key, char8_t* buffer, size_t buf_size, const char8_t* default_value) const override { ScopeGuard guard(*this, key); return readString(buffer, buf_size, default_value); }
	//const char8_t* readString(const char8_t* key, const char8_t* default_value) const override { ScopeGuard guard(*this, key); return readString(default_value); }
	int8_t readInt8(const char8_t* key, int8_t default_value) const override { ScopeGuard guard(*this, key); return readInt8(default_value); }
	uint8_t readUInt8(const char8_t* key, uint8_t default_value) const override { ScopeGuard guard(*this, key); return readUInt8(default_value); }
	int16_t readInt16(const char8_t* key, int16_t default_value) const override { ScopeGuard guard(*this, key); return readInt16(default_value); }
	uint16_t readUInt16(const char8_t* key, uint16_t default_value) const override { ScopeGuard guard(*this, key); return readUInt16(default_value); }
	int32_t readInt32(const char8_t* key, int32_t default_value) const override { ScopeGuard guard(*this, key); return readInt32(default_value); }
	uint32_t readUInt32(const char8_t* key, uint32_t default_value) const override { ScopeGuard guard(*this, key); return readUInt32(default_value); }
	int64_t readInt64(const char8_t* key, int64_t default_value) const override { ScopeGuard guard(*this, key); return readInt64(default_value); }
	uint64_t readUInt64(const char8_t* key, uint64_t default_value) const override { ScopeGuard guard(*this, key); return readUInt64(default_value); }
	float readFloat(const char8_t* key, float default_value) const override { ScopeGuard guard(*this, key); return readFloat(default_value); }
	double readDouble(const char8_t* key, double default_value) const override { ScopeGuard guard(*this, key); return readDouble(default_value); }
	double readNumber(const char8_t* key, double default_value) const override { ScopeGuard guard(*this, key); return readNumber(default_value); }
	bool readBool(const char8_t* key, bool default_value) const override { ScopeGuard guard(*this, key); return readBool(default_value); }

private:
	mutable Vector<Node> _stack;
};

template <class Node>
SerializeReader<Node> MakeSerializeReader(const Node& node, const ProxyAllocator& allocator = ProxyAllocator())
{
	return SerializeReader<Node>(node, allocator);
}

NS_END
