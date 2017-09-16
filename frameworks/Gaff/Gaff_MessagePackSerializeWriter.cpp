/************************************************************************************
Copyright (C) 2017 by Nicholas LaCroix

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

#include "Gaff_MessagePackSerializeWriter.h"

NS_GAFF

bool MessagePackSerializeWriter::init(char** buffer, size_t* size)
{
	return _writer.init(buffer, size);
}

bool MessagePackSerializeWriter::init(char* buffer, size_t size)
{
	return _writer.init(buffer, size);
}

bool MessagePackSerializeWriter::init(const char* filename)
{
	return _writer.init(filename);
}

void MessagePackSerializeWriter::finish(void)
{
	_writer.finish();
}

size_t MessagePackSerializeWriter::size(void) const
{
	return _writer.size();
}

void MessagePackSerializeWriter::startArray(uint32_t size)
{
	_writer.startArray(size);
}

void MessagePackSerializeWriter::endArray(void)
{
	_writer.endArray();
}

void MessagePackSerializeWriter::startObject(uint32_t size)
{
	_writer.startObject(size);
}

void MessagePackSerializeWriter::endObject(void)
{
	_writer.endObject();
}

void MessagePackSerializeWriter::writeKey(const char* key)
{
	_writer.writeKey(key);
}

void MessagePackSerializeWriter::writeInt8(const char* key, int8_t value)
{
	_writer.write(key, value);
}

void MessagePackSerializeWriter::writeInt16(const char* key, int16_t value)
{
	_writer.write(key, value);
}

void MessagePackSerializeWriter::writeInt32(const char* key, int32_t value)
{
	_writer.write(key, value);
}

void MessagePackSerializeWriter::writeInt64(const char* key, int64_t value)
{
	_writer.write(key, value);
}

void MessagePackSerializeWriter::writeUInt8(const char* key, uint8_t value)
{
	_writer.write(key, value);
}

void MessagePackSerializeWriter::writeUInt16(const char* key, uint16_t value)
{
	_writer.write(key, value);
}

void MessagePackSerializeWriter::writeUInt32(const char* key, uint32_t value)
{
	_writer.write(key, value);
}

void MessagePackSerializeWriter::writeUInt64(const char* key, uint64_t value)
{
	_writer.write(key, value);
}

void MessagePackSerializeWriter::writeFloat(const char* key, float value)
{
	_writer.write(key, value);
}

void MessagePackSerializeWriter::writeDouble(const char* key, double value)
{
	_writer.write(key, value);
}

void MessagePackSerializeWriter::writeString(const char* key, const char* value)
{
	_writer.writeUTF8(key, value);
}

void MessagePackSerializeWriter::writeInt8(int8_t value)
{
	_writer.write(value);
}

void MessagePackSerializeWriter::writeInt16(int16_t value)
{
	_writer.write(value);
}

void MessagePackSerializeWriter::writeInt32(int32_t value)
{
	_writer.write(value);
}

void MessagePackSerializeWriter::writeInt64(int64_t value)
{
	_writer.write(value);
}

void MessagePackSerializeWriter::writeUInt8(uint8_t value)
{
	_writer.write(value);
}

void MessagePackSerializeWriter::writeUInt16(uint16_t value)
{
	_writer.write(value);
}

void MessagePackSerializeWriter::writeUInt32(uint32_t value)
{
	_writer.write(value);
}

void MessagePackSerializeWriter::writeUInt64(uint64_t value)
{
	_writer.write(value);
}

void MessagePackSerializeWriter::writeFloat(float value)
{
	_writer.write(value);
}

void MessagePackSerializeWriter::writeDouble(double value)
{
	_writer.write(value);
}

void MessagePackSerializeWriter::writeString(const char* value)
{
	_writer.writeUTF8(value);
}

NS_END
