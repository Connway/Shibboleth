/************************************************************************************
Copyright (C) 2019 by Nicholas LaCroix

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

#include "Gaff_Assert.h"

#ifdef _MSC_VER
	#pragma warning(push)
	#pragma warning(disable : 4127)
#endif

#include <mpack.h>

#ifdef _MSC_VER
	#pragma warning(pop)
#endif

NS_GAFF

class MessagePackNode
{
public:
	template <class Callback>
	bool forEachInObject(Callback&& callback) const
	{
		GAFF_ASSERT(isObject());

		const int32_t length = static_cast<int32_t>(mpack_node_map_count(_node));

		for (int32_t i = 0; i < length; ++i) {
			const size_t index = static_cast<size_t>(i);

			const MessagePackNode key(mpack_node_map_key_at(_node, index));
			const MessagePackNode value(mpack_node_map_value_at(_node, static_cast<size_t>(i)));

			GAFF_ASSERT(key.isString());
			
			if (callback(key.getString(), value)) {
				return true;
			}
		}

		return false;
	}

	template <class Callback>
	bool forEachInArray(Callback&& callback) const
	{
		GAFF_ASSERT(isArray());

		const int32_t length = static_cast<int32_t>(mpack_node_array_length(_node));

		for (int32_t i = 0; i < length; ++i) {
			const MessagePackNode value(mpack_node_array_at(_node, static_cast<size_t>(i)));

			if (callback(i, value)) {
				return true;
			}
		}

		return false;
	}

	bool operator==(const MessagePackNode& rhs) const;
	bool operator!=(const MessagePackNode& rhs) const;

	MessagePackNode operator[](const char* key) const;
	MessagePackNode operator[](int32_t index) const;

	MessagePackNode& operator=(const MessagePackNode& rhs);

	bool isObject(void) const;
	bool isArray(void) const;
	bool isString(void) const;
	bool isNumber(void) const;
	bool isInt32(void) const; // Is(U)Int32/64 for compatability with JSON interface.
	bool isUInt32(void) const;
	bool isInt64(void) const; 
	bool isUInt64(void) const;
	bool isFloat(void) const;
	bool isDouble(void) const;
	bool isBool(void) const;
	bool isTrue(void) const;
	bool isFalse(void) const;
	bool isNull(void) const;

	MessagePackNode getObject(const char* key) const;
	MessagePackNode getObject(int32_t index) const;

	const char* getKey(char* buffer, size_t buf_size, int32_t index) const;
	const char* getKey(int32_t index) const;
	MessagePackNode getValue(int32_t index) const;

	void freeString(const char* str) const;
	int32_t size(void) const;

	const char* getString(char* buffer, size_t buf_size, const char* default_value) const;
	const char* getString(const char* default_value) const;
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

	const char* getString(char* buffer, size_t buf_size) const;
	const char* getString(void) const;
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

	const char* getErrorText(void) const;

private:
	mpack_node_t _node;

	MessagePackNode(mpack_node_t node);
	MessagePackNode(void);

	friend class MessagePackReader;
};

class MessagePackReader
{
public:
	GAFF_MOVE_DEFAULT(MessagePackReader);
	GAFF_NO_COPY(MessagePackReader);

	MessagePackReader(void);
	~MessagePackReader(void);

	bool parse(const char* buffer, size_t size);
	bool openFile(const char* file);

	MessagePackNode getRoot(void) const;

	const char* getErrorText(void) const;

private:
	mpack_tree_t _tree;
	MessagePackNode _root;
	bool _owns_buffer = false;
};

class MessagePackWriter
{
public:
	GAFF_STRUCTORS_DEFAULT(MessagePackWriter);
	GAFF_MOVE_DEFAULT(MessagePackWriter);
	GAFF_NO_COPY(MessagePackWriter);

	bool init(char** buffer, size_t* size);
	bool init(char* buffer, size_t size);
	bool init(const char* filename);
	void finish(void);

	size_t size(void) const;

	template <class T>
	void write(const char* key, T value)
	{
		mpack_write_kv(&_writer, key, value);
	}

	template <class T>
	void write(T value)
	{
		mpack_write(&_writer, value);
	}

	void writeUTF8(const char* key, const char* value);
	void writeUTF8(const char* value);
	void writeKey(const char* value);
	void writeTrue(void);
	void writeFalse(void);
	void writeNull(void);

	void startArray(uint32_t size);
	void endArray(void);

	void startObject(uint32_t size);
	void endObject(void);

private:
	mpack_writer_t _writer;
};

using MPackAllocFunc = void* (*)(size_t);
using MPackFreeFunc = void(*)(void*);

void MessagePackSetMemoryFunctions(MPackAllocFunc alloc, MPackFreeFunc free);

NS_END
