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

template <class Allocator>
JSONSerializeWriter<Allocator>::JSONSerializeWriter(const Allocator& allocator):
	_nodes(allocator)
{
}

template <class Allocator>
const JSON& JSONSerializeWriter<Allocator>::getRootNode(void) const
{
	return _nodes.front();
}

template <class Allocator>
void JSONSerializeWriter<Allocator>::startArray(uint32_t)
{
	if (_nodes.empty()) {
		_nodes.push_back(JSON::CreateArray());
	} else {
		JSON& curr_node = _nodes.back();
		curr_node.setObject(_key, JSON::CreateArray());
		_nodes.push_back(curr_node[_key]);
	}
}

template <class Allocator>
void JSONSerializeWriter<Allocator>::endArray(void)
{
	if (_nodes.size() > 1) {
		_nodes.pop_back();
	}
}

template <class Allocator>
void JSONSerializeWriter<Allocator>::startObject(uint32_t)
{
	if (_nodes.empty()) {
		_nodes.push_back(JSON::CreateObject());
	}
	else {
		JSON& curr_node = _nodes.back();
		curr_node.setObject(_key, JSON::CreateObject());
		_nodes.push_back(curr_node[_key]);
	}
}

template <class Allocator>
void JSONSerializeWriter<Allocator>::endObject(void)
{
	if (_nodes.size() > 1) {
		_nodes.pop_back();
	}
}

template <class Allocator>
void JSONSerializeWriter<Allocator>::writeKey(const char* key)
{
	_key = key;
}

template <class Allocator>
void JSONSerializeWriter<Allocator>::writeInt8(const char* key, int8_t value)
{
	_nodes.back().setObject(key, JSON::CreateInt32(value));
}

template <class Allocator>
void JSONSerializeWriter<Allocator>::writeInt16(const char* key, int16_t value)
{
	_nodes.back().setObject(key, JSON::CreateInt32(value));
}

template <class Allocator>
void JSONSerializeWriter<Allocator>::writeInt32(const char* key, int32_t value)
{
	_nodes.back().setObject(key, JSON::CreateInt32(value));
}

template <class Allocator>
void JSONSerializeWriter<Allocator>::writeInt64(const char* key, int64_t value)
{
	_nodes.back().setObject(key, JSON::CreateInt64(value));
}

template <class Allocator>
void JSONSerializeWriter<Allocator>::writeUInt8(const char* key, uint8_t value)
{
	_nodes.back().setObject(key, JSON::CreateUInt32(value));
}

template <class Allocator>
void JSONSerializeWriter<Allocator>::writeUInt16(const char* key, uint16_t value)
{
	_nodes.back().setObject(key, JSON::CreateUInt32(value));
}

template <class Allocator>
void JSONSerializeWriter<Allocator>::writeUInt32(const char* key, uint32_t value)
{
	_nodes.back().setObject(key, JSON::CreateUInt32(value));
}

template <class Allocator>
void JSONSerializeWriter<Allocator>::writeUInt64(const char* key, uint64_t value)
{
	_nodes.back().setObject(key, JSON::CreateUInt64(value));
}

template <class Allocator>
void JSONSerializeWriter<Allocator>::writeFloat(const char* key, float value)
{
	_nodes.back().setObject(key, JSON::CreateDouble(value));
}

template <class Allocator>
void JSONSerializeWriter<Allocator>::writeDouble(const char* key, double value)
{
	_nodes.back().setObject(key, JSON::CreateDouble(value));
}

template <class Allocator>
void JSONSerializeWriter<Allocator>::writeBool(const char* key, bool value)
{
	_nodes.back().setObject(key, JSON::CreateBool(value));
}

template <class Allocator>
void JSONSerializeWriter<Allocator>::writeString(const char* key, const char* value)
{
	_nodes.back().setObject(key, JSON::CreateString(value));
}

template <class Allocator>
void JSONSerializeWriter<Allocator>::writeInt8(int8_t value)
{
	JSON& curr_node = _nodes.back();
	GAFF_ASSERT(curr_node.isArray());
	curr_node.push(JSON::CreateInt32(value));
}

template <class Allocator>
void JSONSerializeWriter<Allocator>::writeInt16(int16_t value)
{
	JSON& curr_node = _nodes.back();
	GAFF_ASSERT(curr_node.isArray());
	curr_node.push(JSON::CreateInt32(value));
}

template <class Allocator>
void JSONSerializeWriter<Allocator>::writeInt32(int32_t value)
{
	JSON& curr_node = _nodes.back();
	GAFF_ASSERT(curr_node.isArray());
	curr_node.push(JSON::CreateInt32(value));
}

template <class Allocator>
void JSONSerializeWriter<Allocator>::writeInt64(int64_t value)
{
	JSON& curr_node = _nodes.back();
	GAFF_ASSERT(curr_node.isArray());
	curr_node.push(JSON::CreateInt64(value));
}

template <class Allocator>
void JSONSerializeWriter<Allocator>::writeUInt8(uint8_t value)
{
	JSON& curr_node = _nodes.back();
	GAFF_ASSERT(curr_node.isArray());
	curr_node.push(JSON::CreateUInt32(value));
}

template <class Allocator>
void JSONSerializeWriter<Allocator>::writeUInt16(uint16_t value)
{
	JSON& curr_node = _nodes.back();
	GAFF_ASSERT(curr_node.isArray());
	curr_node.push(JSON::CreateUInt32(value));
}

template <class Allocator>
void JSONSerializeWriter<Allocator>::writeUInt32(uint32_t value)
{
	JSON& curr_node = _nodes.back();
	GAFF_ASSERT(curr_node.isArray());
	curr_node.push(JSON::CreateUInt32(value));
}

template <class Allocator>
void JSONSerializeWriter<Allocator>::writeUInt64(uint64_t value)
{
	JSON& curr_node = _nodes.back();
	GAFF_ASSERT(curr_node.isArray());
	curr_node.push(JSON::CreateUInt64(value));
}

template <class Allocator>
void JSONSerializeWriter<Allocator>::writeFloat(float value)
{
	JSON& curr_node = _nodes.back();
	GAFF_ASSERT(curr_node.isArray());
	curr_node.push(JSON::CreateDouble(value));
}

template <class Allocator>
void JSONSerializeWriter<Allocator>::writeDouble(double value)
{
	JSON& curr_node = _nodes.back();
	GAFF_ASSERT(curr_node.isArray());
	curr_node.push(JSON::CreateDouble(value));
}

template <class Allocator>
void JSONSerializeWriter<Allocator>::writeBool(bool value)
{
	JSON& curr_node = _nodes.back();
	GAFF_ASSERT(curr_node.isArray());
	curr_node.push(JSON::CreateBool(value));
}

template <class Allocator>
void JSONSerializeWriter<Allocator>::writeString(const char* value)
{
	JSON& curr_node = _nodes.back();
	GAFF_ASSERT(curr_node.isArray());
	curr_node.push(JSON::CreateString(value));
}
