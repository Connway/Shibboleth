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

#include "Shibboleth_JSONSerializeWriter.h"

NS_SHIBBOLETH

JSONSerializeWriter::JSONSerializeWriter(const ProxyAllocator& allocator):
	_nodes(allocator)
{
}

const Gaff::JSON& JSONSerializeWriter::getRootNode(void) const
{
	return _nodes.front();
}

void JSONSerializeWriter::startArray(uint32_t)
{
	if (_nodes.empty()) {
		_nodes.push_back(Gaff::JSON::CreateArray());
	} else {
		Gaff::JSON& curr_node = _nodes.back();
		curr_node.setObject(_key, Gaff::JSON::CreateArray());
		_nodes.push_back(curr_node[_key]);
	}
}

void JSONSerializeWriter::endArray(void)
{
	if (_nodes.size() > 1) {
		_nodes.pop_back();
	}
}

void JSONSerializeWriter::startObject(uint32_t)
{
	if (_nodes.empty()) {
		_nodes.push_back(Gaff::JSON::CreateObject());
	}
	else {
		Gaff::JSON& curr_node = _nodes.back();
		curr_node.setObject(_key, Gaff::JSON::CreateObject());
		_nodes.push_back(curr_node[_key]);
	}
}

void JSONSerializeWriter::endObject(void)
{
	if (_nodes.size() > 1) {
		_nodes.pop_back();
	}
}

void JSONSerializeWriter::writeKey(const char8_t* key)
{
	_key = key;
}

void JSONSerializeWriter::writeInt8(const char8_t* key, int8_t value)
{
	_nodes.back().setObject(key, Gaff::JSON::CreateInt32(value));
}

void JSONSerializeWriter::writeInt16(const char8_t* key, int16_t value)
{
	_nodes.back().setObject(key, Gaff::JSON::CreateInt32(value));
}

void JSONSerializeWriter::writeInt32(const char8_t* key, int32_t value)
{
	_nodes.back().setObject(key, Gaff::JSON::CreateInt32(value));
}

void JSONSerializeWriter::writeInt64(const char8_t* key, int64_t value)
{
	_nodes.back().setObject(key, Gaff::JSON::CreateInt64(value));
}

void JSONSerializeWriter::writeUInt8(const char8_t* key, uint8_t value)
{
	_nodes.back().setObject(key, Gaff::JSON::CreateUInt32(value));
}

void JSONSerializeWriter::writeUInt16(const char8_t* key, uint16_t value)
{
	_nodes.back().setObject(key, Gaff::JSON::CreateUInt32(value));
}

void JSONSerializeWriter::writeUInt32(const char8_t* key, uint32_t value)
{
	_nodes.back().setObject(key, Gaff::JSON::CreateUInt32(value));
}

void JSONSerializeWriter::writeUInt64(const char8_t* key, uint64_t value)
{
	_nodes.back().setObject(key, Gaff::JSON::CreateUInt64(value));
}

void JSONSerializeWriter::writeFloat(const char8_t* key, float value)
{
	_nodes.back().setObject(key, Gaff::JSON::CreateDouble(value));
}

void JSONSerializeWriter::writeDouble(const char8_t* key, double value)
{
	_nodes.back().setObject(key, Gaff::JSON::CreateDouble(value));
}

void JSONSerializeWriter::writeBool(const char8_t* key, bool value)
{
	_nodes.back().setObject(key, Gaff::JSON::CreateBool(value));
}

void JSONSerializeWriter::writeString(const char8_t* key, const char8_t* value)
{
	_nodes.back().setObject(key, Gaff::JSON::CreateString(value));
}

void JSONSerializeWriter::writeInt8(int8_t value)
{
	Gaff::JSON& curr_node = _nodes.back();
	GAFF_ASSERT(curr_node.isArray());
	curr_node.push(Gaff::JSON::CreateInt32(value));
}

void JSONSerializeWriter::writeInt16(int16_t value)
{
	Gaff::JSON& curr_node = _nodes.back();
	GAFF_ASSERT(curr_node.isArray());
	curr_node.push(Gaff::JSON::CreateInt32(value));
}

void JSONSerializeWriter::writeInt32(int32_t value)
{
	Gaff::JSON& curr_node = _nodes.back();
	GAFF_ASSERT(curr_node.isArray());
	curr_node.push(Gaff::JSON::CreateInt32(value));
}

void JSONSerializeWriter::writeInt64(int64_t value)
{
	Gaff::JSON& curr_node = _nodes.back();
	GAFF_ASSERT(curr_node.isArray());
	curr_node.push(Gaff::JSON::CreateInt64(value));
}

void JSONSerializeWriter::writeUInt8(uint8_t value)
{
	Gaff::JSON& curr_node = _nodes.back();
	GAFF_ASSERT(curr_node.isArray());
	curr_node.push(Gaff::JSON::CreateUInt32(value));
}

void JSONSerializeWriter::writeUInt16(uint16_t value)
{
	Gaff::JSON& curr_node = _nodes.back();
	GAFF_ASSERT(curr_node.isArray());
	curr_node.push(Gaff::JSON::CreateUInt32(value));
}

void JSONSerializeWriter::writeUInt32(uint32_t value)
{
	Gaff::JSON& curr_node = _nodes.back();
	GAFF_ASSERT(curr_node.isArray());
	curr_node.push(Gaff::JSON::CreateUInt32(value));
}

void JSONSerializeWriter::writeUInt64(uint64_t value)
{
	Gaff::JSON& curr_node = _nodes.back();
	GAFF_ASSERT(curr_node.isArray());
	curr_node.push(Gaff::JSON::CreateUInt64(value));
}

void JSONSerializeWriter::writeFloat(float value)
{
	Gaff::JSON& curr_node = _nodes.back();
	GAFF_ASSERT(curr_node.isArray());
	curr_node.push(Gaff::JSON::CreateDouble(value));
}

void JSONSerializeWriter::writeDouble(double value)
{
	Gaff::JSON& curr_node = _nodes.back();
	GAFF_ASSERT(curr_node.isArray());
	curr_node.push(Gaff::JSON::CreateDouble(value));
}

void JSONSerializeWriter::writeBool(bool value)
{
	Gaff::JSON& curr_node = _nodes.back();
	GAFF_ASSERT(curr_node.isArray());
	curr_node.push(Gaff::JSON::CreateBool(value));
}

void JSONSerializeWriter::writeString(const char8_t* value)
{
	Gaff::JSON& curr_node = _nodes.back();
	GAFF_ASSERT(curr_node.isArray());
	curr_node.push(Gaff::JSON::CreateString(value));
}

NS_END
