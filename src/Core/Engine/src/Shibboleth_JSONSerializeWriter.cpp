/************************************************************************************
Copyright (C) 2024 by Nicholas LaCroix

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

		GAFF_ASSERT(curr_node.isArray() || curr_node.isObject());

		if (curr_node.isArray()) {
			const int32_t size = curr_node.size();
			curr_node.push(Gaff::JSON::CreateArray());
			_nodes.push_back(curr_node.getObject(size));

		} else {
			GAFF_ASSERT(_key);

			curr_node.setObject(_key, Gaff::JSON::CreateArray());
			_nodes.push_back(curr_node.getObject(_key));

			_key = nullptr;
		}
	}
}

void JSONSerializeWriter::endArray(void)
{
	GAFF_ASSERT(_nodes.size() > 1);
	_nodes.pop_back();
}

void JSONSerializeWriter::startObject(uint32_t)
{
	if (_nodes.empty()) {
		_nodes.push_back(Gaff::JSON::CreateObject());
	}
	else {
		Gaff::JSON& curr_node = _nodes.back();

		GAFF_ASSERT(curr_node.isArray() || curr_node.isObject());

		if (curr_node.isArray()) {
			const int32_t size = curr_node.size();
			curr_node.push(Gaff::JSON::CreateObject());
			_nodes.push_back(curr_node.getObject(size));

		} else {
			GAFF_ASSERT(_key);

			curr_node.setObject(_key, Gaff::JSON::CreateObject());
			_nodes.push_back(curr_node.getObject(_key));

			_key = nullptr;
		}
	}
}

void JSONSerializeWriter::endObject(void)
{
	GAFF_ASSERT(_nodes.size() > 1);
	_nodes.pop_back();
}

void JSONSerializeWriter::writeKey(const char8_t* key)
{
	_key = key;
}

void JSONSerializeWriter::writeInt8(const char8_t* key, int8_t value)
{
	GAFF_ASSERT(!_nodes.empty());
	_nodes.back().setObject(key, Gaff::JSON::CreateInt32(value));
}

void JSONSerializeWriter::writeInt16(const char8_t* key, int16_t value)
{
	GAFF_ASSERT(!_nodes.empty());
	_nodes.back().setObject(key, Gaff::JSON::CreateInt32(value));
}

void JSONSerializeWriter::writeInt32(const char8_t* key, int32_t value)
{
	GAFF_ASSERT(!_nodes.empty());
	_nodes.back().setObject(key, Gaff::JSON::CreateInt32(value));
}

void JSONSerializeWriter::writeInt64(const char8_t* key, int64_t value)
{
	GAFF_ASSERT(!_nodes.empty());
	_nodes.back().setObject(key, Gaff::JSON::CreateInt64(value));
}

void JSONSerializeWriter::writeUInt8(const char8_t* key, uint8_t value)
{
	GAFF_ASSERT(!_nodes.empty());
	_nodes.back().setObject(key, Gaff::JSON::CreateUInt32(value));
}

void JSONSerializeWriter::writeUInt16(const char8_t* key, uint16_t value)
{
	GAFF_ASSERT(!_nodes.empty());
	_nodes.back().setObject(key, Gaff::JSON::CreateUInt32(value));
}

void JSONSerializeWriter::writeUInt32(const char8_t* key, uint32_t value)
{
	GAFF_ASSERT(!_nodes.empty());
	_nodes.back().setObject(key, Gaff::JSON::CreateUInt32(value));
}

void JSONSerializeWriter::writeUInt64(const char8_t* key, uint64_t value)
{
	GAFF_ASSERT(!_nodes.empty());
	_nodes.back().setObject(key, Gaff::JSON::CreateUInt64(value));
}

void JSONSerializeWriter::writeFloat(const char8_t* key, float value)
{
	GAFF_ASSERT(!_nodes.empty());
	_nodes.back().setObject(key, Gaff::JSON::CreateDouble(value));
}

void JSONSerializeWriter::writeDouble(const char8_t* key, double value)
{
	GAFF_ASSERT(!_nodes.empty());
	_nodes.back().setObject(key, Gaff::JSON::CreateDouble(value));
}

void JSONSerializeWriter::writeBool(const char8_t* key, bool value)
{
	GAFF_ASSERT(!_nodes.empty());
	_nodes.back().setObject(key, Gaff::JSON::CreateBool(value));
}

void JSONSerializeWriter::writeString(const char8_t* key, const char8_t* value)
{
	GAFF_ASSERT(!_nodes.empty());
	_nodes.back().setObject(key, Gaff::JSON::CreateString(value));
}

void JSONSerializeWriter::writeNull(const char8_t* key)
{
	GAFF_ASSERT(!_nodes.empty());
	_nodes.back().setObject(key, Gaff::JSON::CreateNull());
}

void JSONSerializeWriter::writeInt8(int8_t value)
{
	GAFF_ASSERT(!_nodes.empty());
	Gaff::JSON& curr_node = _nodes.back();
	GAFF_ASSERT(curr_node.isArray() || curr_node.isObject());

	if (curr_node.isArray()) {
		curr_node.push(Gaff::JSON::CreateInt32(value));

	} else {
		GAFF_ASSERT(_key);
		curr_node.setObject(_key, Gaff::JSON::CreateInt32(value));
		_key = nullptr;
	}
}

void JSONSerializeWriter::writeInt16(int16_t value)
{
	GAFF_ASSERT(!_nodes.empty());
	Gaff::JSON& curr_node = _nodes.back();
	GAFF_ASSERT(curr_node.isArray() || curr_node.isObject());

	if (curr_node.isArray()) {
		curr_node.push(Gaff::JSON::CreateInt32(value));

	} else {
		GAFF_ASSERT(_key);
		curr_node.setObject(_key, Gaff::JSON::CreateInt32(value));
		_key = nullptr;
	}
}

void JSONSerializeWriter::writeInt32(int32_t value)
{
	GAFF_ASSERT(!_nodes.empty());
	Gaff::JSON& curr_node = _nodes.back();
	GAFF_ASSERT(curr_node.isArray() || curr_node.isObject());

	if (curr_node.isArray()) {
		curr_node.push(Gaff::JSON::CreateInt32(value));

	} else {
		GAFF_ASSERT(_key);
		curr_node.setObject(_key, Gaff::JSON::CreateInt32(value));
		_key = nullptr;
	}
}

void JSONSerializeWriter::writeInt64(int64_t value)
{
	GAFF_ASSERT(!_nodes.empty());
	Gaff::JSON& curr_node = _nodes.back();
	GAFF_ASSERT(curr_node.isArray() || curr_node.isObject());

	if (curr_node.isArray()) {
		curr_node.push(Gaff::JSON::CreateInt64(value));

	} else {
		GAFF_ASSERT(_key);
		curr_node.setObject(_key, Gaff::JSON::CreateInt64(value));
		_key = nullptr;
	}
}

void JSONSerializeWriter::writeUInt8(uint8_t value)
{
	GAFF_ASSERT(!_nodes.empty());
	Gaff::JSON& curr_node = _nodes.back();
	GAFF_ASSERT(curr_node.isArray() || curr_node.isObject());

	if (curr_node.isArray()) {
		curr_node.push(Gaff::JSON::CreateUInt32(value));

	} else {
		GAFF_ASSERT(_key);
		curr_node.setObject(_key, Gaff::JSON::CreateUInt32(value));
		_key = nullptr;
	}
}

void JSONSerializeWriter::writeUInt16(uint16_t value)
{
	GAFF_ASSERT(!_nodes.empty());
	Gaff::JSON& curr_node = _nodes.back();
	GAFF_ASSERT(curr_node.isArray() || curr_node.isObject());

	if (curr_node.isArray()) {
		curr_node.push(Gaff::JSON::CreateUInt32(value));

	} else {
		GAFF_ASSERT(_key);
		curr_node.setObject(_key, Gaff::JSON::CreateUInt32(value));
		_key = nullptr;
	}
}

void JSONSerializeWriter::writeUInt32(uint32_t value)
{
	GAFF_ASSERT(!_nodes.empty());
	Gaff::JSON& curr_node = _nodes.back();
	GAFF_ASSERT(curr_node.isArray() || curr_node.isObject());

	if (curr_node.isArray()) {
		curr_node.push(Gaff::JSON::CreateUInt32(value));

	} else {
		GAFF_ASSERT(_key);
		curr_node.setObject(_key, Gaff::JSON::CreateUInt32(value));
		_key = nullptr;
	}
}

void JSONSerializeWriter::writeUInt64(uint64_t value)
{
	GAFF_ASSERT(!_nodes.empty());
	Gaff::JSON& curr_node = _nodes.back();
	GAFF_ASSERT(curr_node.isArray() || curr_node.isObject());

	if (curr_node.isArray()) {
		curr_node.push(Gaff::JSON::CreateUInt64(value));

	} else {
		GAFF_ASSERT(_key);
		curr_node.setObject(_key, Gaff::JSON::CreateUInt64(value));
		_key = nullptr;
	}
}

void JSONSerializeWriter::writeFloat(float value)
{
	GAFF_ASSERT(!_nodes.empty());
	Gaff::JSON& curr_node = _nodes.back();
	GAFF_ASSERT(curr_node.isArray() || curr_node.isObject());

	if (curr_node.isArray()) {
		curr_node.push(Gaff::JSON::CreateDouble(value));

	} else {
		GAFF_ASSERT(_key);
		curr_node.setObject(_key, Gaff::JSON::CreateDouble(value));
		_key = nullptr;
	}
}

void JSONSerializeWriter::writeDouble(double value)
{
	GAFF_ASSERT(!_nodes.empty());
	Gaff::JSON& curr_node = _nodes.back();
	GAFF_ASSERT(curr_node.isArray() || curr_node.isObject());

	if (curr_node.isArray()) {
		curr_node.push(Gaff::JSON::CreateDouble(value));

	} else {
		GAFF_ASSERT(_key);
		curr_node.setObject(_key, Gaff::JSON::CreateDouble(value));
		_key = nullptr;
	}
}

void JSONSerializeWriter::writeBool(bool value)
{
	GAFF_ASSERT(!_nodes.empty());
	Gaff::JSON& curr_node = _nodes.back();
	GAFF_ASSERT(curr_node.isArray() || curr_node.isObject());

	if (curr_node.isArray()) {
		curr_node.push(Gaff::JSON::CreateBool(value));

	} else {
		GAFF_ASSERT(_key);
		curr_node.setObject(_key, Gaff::JSON::CreateBool(value));
		_key = nullptr;
	}
}

void JSONSerializeWriter::writeString(const char8_t* value)
{
	GAFF_ASSERT(!_nodes.empty());
	Gaff::JSON& curr_node = _nodes.back();
	GAFF_ASSERT(curr_node.isArray() || curr_node.isObject());

	if (curr_node.isArray()) {
		curr_node.push(Gaff::JSON::CreateString(value));

	} else {
		GAFF_ASSERT(_key);
		curr_node.setObject(_key, Gaff::JSON::CreateString(value));
		_key = nullptr;
	}
}

void JSONSerializeWriter::writeNull(void)
{
	GAFF_ASSERT(!_nodes.empty());
	Gaff::JSON& curr_node = _nodes.back();
	GAFF_ASSERT(curr_node.isArray() || curr_node.isObject());

	if (curr_node.isArray()) {
		curr_node.push(Gaff::JSON::CreateNull());

	} else {
		GAFF_ASSERT(_key);
		curr_node.setObject(_key, Gaff::JSON::CreateNull());
		_key = nullptr;
	}
}

NS_END
