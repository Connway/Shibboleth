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

#include "Gaff_MessagePack.h"

NS_GAFF

MessagePackNode::MessagePackNode(mpack_node_t node):
	_node(node)
{
}

MessagePackNode::MessagePackNode(void):
	_node({ nullptr, nullptr })
{
}

bool MessagePackNode::operator==(const MessagePackNode& rhs) const
{
	return _node.data == rhs._node.data && _node.tree == rhs._node.tree;
}

bool MessagePackNode::operator!=(const MessagePackNode& rhs) const
{
	return _node.data != rhs._node.data || _node.tree != rhs._node.tree;
}

MessagePackNode MessagePackNode::operator[](const char* key) const
{
	GAFF_ASSERT(isObject());
	return MessagePackNode(mpack_node_map_cstr_optional(_node, key));
}

MessagePackNode MessagePackNode::operator[](int32_t index) const
{
	GAFF_ASSERT(isArray() && index < size());
	return MessagePackNode(mpack_node_array_at(_node, static_cast<size_t>(index)));
}

bool MessagePackNode::isObject(void) const
{
	return _node.data->type == mpack_type_map;
}

bool MessagePackNode::isArray(void) const
{
	return _node.data->type == mpack_type_array;
}

bool MessagePackNode::isString(void) const
{
	return _node.data->type == mpack_type_str;
}

bool MessagePackNode::isNumber(void) const
{
	return isInt64() || isUInt64() || isFloat() || isDouble();
}

bool MessagePackNode::isInt8(void) const
{
	return _node.data->type == mpack_type_int;
}

bool MessagePackNode::isUInt8(void) const
{
	return _node.data->type == mpack_type_uint;
}

bool MessagePackNode::isInt16(void) const
{
	return _node.data->type == mpack_type_int;
}

bool MessagePackNode::isUInt16(void) const
{
	return _node.data->type == mpack_type_uint;
}

bool MessagePackNode::isInt32(void) const
{
	return _node.data->type == mpack_type_int;
}

bool MessagePackNode::isUInt32(void) const
{
	return _node.data->type == mpack_type_uint;
}

bool MessagePackNode::isInt64(void) const
{
	return _node.data->type == mpack_type_int;
}

bool MessagePackNode::isUInt64(void) const
{
	return _node.data->type == mpack_type_uint;
}

bool MessagePackNode::isFloat(void) const
{
	return _node.data->type == mpack_type_float;
}

bool MessagePackNode::isDouble(void) const
{
	return _node.data->type == mpack_type_double;
}

bool MessagePackNode::isBool(void) const
{
	return _node.data->type == mpack_type_bool;
}

bool MessagePackNode::isTrue(void) const
{
	const bool ret = isBool() && mpack_node_bool(_node);
	GAFF_ASSERT(_node.tree->error == mpack_ok);
	return ret;
}

bool MessagePackNode::isFalse(void) const
{
	const bool ret = isBool() && !mpack_node_bool(_node);
	GAFF_ASSERT(_node.tree->error == mpack_ok);
	return ret;
}

bool MessagePackNode::isNull(void) const
{
	return _node.data->type == mpack_type_nil;
}

MessagePackNode MessagePackNode::getObject(const char* key) const
{
	GAFF_ASSERT(isObject());
	return MessagePackNode(mpack_node_map_cstr_optional(_node, key));
}

MessagePackNode MessagePackNode::getObject(int32_t index) const
{
	GAFF_ASSERT(isArray() && index < size());
	return MessagePackNode(mpack_node_map_value_at(_node, static_cast<size_t>(index)));
}

const char* MessagePackNode::getKey(char* buffer, size_t buf_size, int32_t index) const
{
	GAFF_ASSERT(isObject() && index < size());

	mpack_node_t node = mpack_node_map_key_at(_node, static_cast<size_t>(index));
	GAFF_ASSERT(node.data->type == mpack_type_str);

	mpack_node_copy_utf8(node, buffer, buf_size);
	GAFF_ASSERT(_node.tree->error == mpack_ok);

	return buffer;
}

const char* MessagePackNode::getKey(int32_t index) const
{
	GAFF_ASSERT(isObject() && index < size());

	mpack_node_t node = mpack_node_map_key_at(_node, static_cast<size_t>(index));
	GAFF_ASSERT(node.data->type == mpack_type_str);

	const char* const ret = mpack_node_utf8_cstr_alloc(node, node.data->len + 1);
	GAFF_ASSERT(node.tree->error == mpack_ok);

	return ret;
}

MessagePackNode MessagePackNode::getValue(int32_t index) const
{
	GAFF_ASSERT(isObject() && index < size());
	return MessagePackNode(mpack_node_map_value_at(_node, static_cast<size_t>(index)));
}

void MessagePackNode::freeString(const char* str) const
{
	MPACK_FREE(const_cast<char*>(str));
}

int32_t MessagePackNode::size(void) const
{
	GAFF_ASSERT(isString() || isArray() || isObject());

	if (isString()) {
		return static_cast<int32_t>(mpack_node_strlen(_node));
	} else if (isArray()) {
		return static_cast<int32_t>(mpack_node_array_length(_node));
	}

	return static_cast<int32_t>(mpack_node_map_count(_node));
}

const char* MessagePackNode::getString(char* buffer, size_t buf_size, const char* default_value) const
{
	GAFF_ASSERT(isString() || isNull());

	if (isNull()) {
		return default_value;
	}

	mpack_node_copy_utf8(_node, buffer, buf_size);
	GAFF_ASSERT(_node.tree->error == mpack_ok);

	return buffer;
}

const char* MessagePackNode::getString(const char* default_value) const
{
	GAFF_ASSERT(isString() || isNull());

	if (isNull()) {
		return default_value;
	}

	const char* const ret = mpack_node_utf8_cstr_alloc(_node, _node.data->len + 1);
	GAFF_ASSERT(_node.tree->error == mpack_ok);

	return ret;
}

int8_t MessagePackNode::getInt8(int8_t default_value) const
{
	if (isNull()) {
		return default_value;
	}

	const int8_t ret = mpack_node_i8(_node);
	GAFF_ASSERT(_node.tree->error == mpack_ok);

	return ret;
}

uint8_t MessagePackNode::getUInt8(uint8_t default_value) const
{
	if (isNull()) {
		return default_value;
	}

	const uint8_t ret = mpack_node_u8(_node);
	GAFF_ASSERT(_node.tree->error == mpack_ok);
	return ret;
}

int16_t MessagePackNode::getInt16(int16_t default_value) const
{
	if (isNull()) {
		return default_value;
	}

	const int16_t ret = mpack_node_i16(_node);
	GAFF_ASSERT(_node.tree->error == mpack_ok);
	return ret;
}

uint16_t MessagePackNode::getUInt16(uint16_t default_value) const
{
	if (isNull()) {
		return default_value;
	}

	const uint16_t ret = mpack_node_u16(_node);
	GAFF_ASSERT(_node.tree->error == mpack_ok);
	return ret;
}

int32_t MessagePackNode::getInt32(int32_t default_value) const
{
	if (isNull()) {
		return default_value;
	}

	const int32_t ret = mpack_node_i32(_node);
	GAFF_ASSERT(_node.tree->error == mpack_ok);
	return ret;
}

uint32_t MessagePackNode::getUInt32(uint32_t default_value) const
{
	if (isNull()) {
		return default_value;
	}

	const uint32_t ret = mpack_node_u32(_node);
	GAFF_ASSERT(_node.tree->error == mpack_ok);
	return ret;
}

int64_t MessagePackNode::getInt64(int64_t default_value) const
{
	if (isNull()) {
		return default_value;
	}

	const int64_t ret = mpack_node_i64(_node);
	GAFF_ASSERT(_node.tree->error == mpack_ok);
	return ret;
}

uint64_t MessagePackNode::getUInt64(uint64_t default_value) const
{
	if (isNull()) {
		return default_value;
	}
	
	const uint64_t ret = mpack_node_u64(_node);
	GAFF_ASSERT(_node.tree->error == mpack_ok);
	return ret;
}

float MessagePackNode::getFloat(float default_value) const
{
	if (isNull()) {
		return default_value;
	}

	const float ret = mpack_node_float(_node);
	GAFF_ASSERT(_node.tree->error == mpack_ok);
	return ret;
}

double MessagePackNode::getDouble(double default_value) const
{
	if (isNull()) {
		return default_value;
	}

	const double ret = mpack_node_double(_node);
	GAFF_ASSERT(_node.tree->error == mpack_ok);
	return ret;
}

double MessagePackNode::getNumber(double default_value) const
{
	if (isNull()) {
		return default_value;
	}

	double ret = 0.0;

	if (isInt64()) {
		ret = static_cast<double>(mpack_node_i64(_node));
	}
	
	if (isUInt64()) {
		ret = static_cast<double>(mpack_node_u64(_node));
	}
	
	if (isFloat()) {
		ret = static_cast<double>(mpack_node_float(_node));
	}
	
	if (isDouble()) {
		ret = mpack_node_double(_node);
	}

	GAFF_ASSERT(_node.tree->error == mpack_ok);
	return ret;
}

bool MessagePackNode::getBool(bool default_value) const
{
	if (isNull()) {
		return default_value;
	}

	const bool ret = mpack_node_bool(_node);
	GAFF_ASSERT(_node.tree->error == mpack_ok);
	return ret;
}

const char* MessagePackNode::getString(char* buffer, size_t buf_size) const
{
	GAFF_ASSERT(isString());
	mpack_node_copy_utf8(_node, buffer, buf_size);
	GAFF_ASSERT(_node.tree->error == mpack_ok);
	return buffer;
}

const char* MessagePackNode::getString(void) const
{
	GAFF_ASSERT(isString());

	const char* const ret = mpack_node_utf8_cstr_alloc(_node, _node.data->len + 1);
	GAFF_ASSERT(_node.tree->error == mpack_ok);

	return ret;
}

int8_t MessagePackNode::getInt8(void) const
{
	const int8_t ret = mpack_node_i8(_node);
	GAFF_ASSERT(_node.tree->error == mpack_ok);
	return ret;
}

uint8_t MessagePackNode::getUInt8(void) const
{
	const uint8_t ret = mpack_node_u8(_node);
	GAFF_ASSERT(_node.tree->error == mpack_ok);
	return ret;
}

int16_t MessagePackNode::getInt16(void) const
{
	const int16_t ret = mpack_node_i16(_node);
	GAFF_ASSERT(_node.tree->error == mpack_ok);
	return ret;
}

uint16_t MessagePackNode::getUInt16(void) const
{
	const uint16_t ret = mpack_node_u16(_node);
	GAFF_ASSERT(_node.tree->error == mpack_ok);
	return ret;
}

int32_t MessagePackNode::getInt32(void) const
{
	const int32_t ret = mpack_node_i32(_node);
	GAFF_ASSERT(_node.tree->error == mpack_ok);
	return ret;
}

uint32_t MessagePackNode::getUInt32(void) const
{
	const uint32_t ret = mpack_node_u32(_node);
	GAFF_ASSERT(_node.tree->error == mpack_ok);
	return ret;
}

int64_t MessagePackNode::getInt64(void) const
{
	const int64_t ret = mpack_node_i64(_node);
	GAFF_ASSERT(_node.tree->error == mpack_ok);
	return ret;
}

uint64_t MessagePackNode::getUInt64(void) const
{
	const uint64_t ret = mpack_node_u64(_node);
	GAFF_ASSERT(_node.tree->error == mpack_ok);
	return ret;
}

float MessagePackNode::getFloat(void) const
{
	const float ret = mpack_node_float(_node);
	GAFF_ASSERT(_node.tree->error == mpack_ok);
	return ret;
}

double MessagePackNode::getDouble(void) const
{
	const double ret = mpack_node_double(_node);
	GAFF_ASSERT(_node.tree->error == mpack_ok);
	return ret;
}

double MessagePackNode::getNumber(void) const
{
	double ret = 0.0;

	if (isInt64()) {
		ret = static_cast<double>(mpack_node_i64(_node));
	}

	if (isUInt64()) {
		ret = static_cast<double>(mpack_node_u64(_node));
	}

	if (isFloat()) {
		ret = static_cast<double>(mpack_node_float(_node));
	}

	if (isDouble()) {
		ret = mpack_node_double(_node);
	}

	GAFF_ASSERT(_node.tree->error == mpack_ok);
	return ret;
}

bool MessagePackNode::getBool(void) const
{
	const bool ret = mpack_node_bool(_node);
	GAFF_ASSERT(_node.tree->error == mpack_ok);
	return ret;
}

const char* MessagePackNode::getErrorText(void) const
{
	return mpack_error_to_string(_node.tree->error);
}



MessagePackReader::MessagePackReader(void)
{
	mpack_memset(&_tree, 0, sizeof(_tree));
	_tree.nil_node.type = mpack_type_nil;
	_tree.missing_node.type = mpack_type_missing;
	_tree.max_size = SIZE_MAX;
	_tree.max_nodes = SIZE_MAX;
}

MessagePackReader::~MessagePackReader(void)
{
	if (!_owns_buffer) {
		_tree.buffer = nullptr;
	}

	mpack_tree_destroy(&_tree);
}

bool MessagePackReader::parse(const char* buffer, size_t size, bool take_ownership)
{
	mpack_tree_init_data(&_tree, buffer, size);
	_owns_buffer = take_ownership;

	if (_tree.error != mpack_ok) {
		return false;
	}

	mpack_tree_parse(&_tree);

	if (_tree.error != mpack_ok) {
		return false;
	}

	_root._node = mpack_tree_root(&_tree);
	return _tree.error == mpack_ok;
}

bool MessagePackReader::openFile(const char* file)
{
	mpack_tree_init_filename(&_tree, file, 0);

	if (_tree.error != mpack_ok) {
		return false;
	}

	mpack_tree_parse(&_tree);

	if (_tree.error != mpack_ok) {
		return false;
	}

	_root._node = mpack_tree_root(&_tree);
	_owns_buffer = true;

	return _tree.error == mpack_ok;
}

MessagePackNode MessagePackReader::getRoot(void) const
{
	return _root;
}

const char* MessagePackReader::getErrorText(void) const
{
	return mpack_error_to_string(_tree.error);
}



bool MessagePackWriter::init(char** buffer, size_t* size)
{
	mpack_writer_init_growable(&_writer, buffer, size);
	return _writer.error == mpack_ok;
}

bool MessagePackWriter::init(char* buffer, size_t size)
{
	mpack_writer_init(&_writer, buffer, size);
	return _writer.error == mpack_ok;
}

bool MessagePackWriter::init(const char* filename)
{
	mpack_writer_init_filename(&_writer, filename);
	return _writer.error == mpack_ok;
}

void MessagePackWriter::finish(void)
{
	mpack_writer_destroy(&_writer);
}

size_t MessagePackWriter::size(void) const
{
	return mpack_writer_buffer_used(const_cast<mpack_writer_t*>(&_writer));
}

void MessagePackWriter::writeUTF8(const char* key, const char* value)
{
	mpack_write_cstr(&_writer, key);
	mpack_write_utf8_cstr_or_nil(&_writer, value);
}

void MessagePackWriter::writeUTF8(const char* value)
{
	mpack_write_utf8_cstr_or_nil(&_writer, value);
}

void MessagePackWriter::writeKey(const char* value)
{
	mpack_write_cstr(&_writer, value);
}

void MessagePackWriter::writeTrue(void)
{
	mpack_write_true(&_writer);
}

void MessagePackWriter::writeFalse(void)
{
	mpack_write_false(&_writer);
}

void MessagePackWriter::writeNull(void)
{
	mpack_write_nil(&_writer);
}

void MessagePackWriter::startArray(uint32_t size)
{
	mpack_start_array(&_writer, size);
}

void MessagePackWriter::endArray(void)
{
	mpack_finish_array(&_writer);
}

void MessagePackWriter::startObject(uint32_t size)
{
	mpack_start_map(&_writer, size);
}

void MessagePackWriter::endObject(void)
{
	mpack_finish_map(&_writer);
}



static MPackAllocFunc g_alloc = malloc;
static MPackFreeFunc g_free = free;

void MessagePackSetMemoryFunctions(MPackAllocFunc alloc, MPackFreeFunc free)
{
	GAFF_ASSERT(alloc);
	GAFF_ASSERT(free);
	g_alloc = alloc;
	g_free = free;
}

NS_END

extern "C" void* MPackInternalAlloc(size_t size)
{
	return Gaff::g_alloc(size);
}	

extern "C" void MPackInternalFree(void* data)
{
	Gaff::g_free(data);
}
