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

#pragma once

#include "Gaff_SerializeInterfaces.h"
#include "Gaff_MessagePack.h"
#include "Gaff_Vector.h"
#include "Gaff_Assert.h"

NS_GAFF

class MessagePackSerializeWriter : public ISerializeWriter
{
public:
	void startArray(void);
	void endArray(void);

	void startObject(void);
	void endObject(void);

	void writeInt8(const char* key, int8_t value);
	void writeInt16(const char* key, int16_t value);
	void writeInt32(const char* key, int32_t value);
	void writeInt64(const char* key, int64_t value);

	void writeUInt8(const char* key, uint8_t value);
	void writeUInt16(const char* key, uint16_t value);
	void writeUInt32(const char* key, uint32_t value);
	void writeUInt64(const char* key, uint64_t value);

	void writeFloat(const char* key, float value);
	void writeDouble(const char* key, double value);

	void writeString(const char* key, const char* value, size_t size);
	void writeString(const char* key, const char* value);

	void writeInt8(int8_t value);
	void writeInt16(int16_t value);
	void writeInt32(int32_t value);
	void writeInt64(int64_t value);

	void writeUInt8(uint8_t value);
	void writeUInt16(uint16_t value);
	void writeUInt32(uint32_t value);
	void writeUInt64(uint64_t value);

	void writeFloat(float value);
	void writeDouble(double value);

	void writeString(const char* value, size_t size);
	void writeString(const char* value);

private:
	MessagePackWriter _writer;
};

NS_END
