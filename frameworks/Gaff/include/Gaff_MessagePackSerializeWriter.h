/************************************************************************************
Copyright (C) 2018 by Nicholas LaCroix

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
	bool init(char** buffer, size_t* size);
	bool init(char* buffer, size_t size);
	bool init(const char* filename);

	void finish(void);

	size_t size(void) const;

	void startArray(uint32_t size) override;
	void endArray(void) override;

	void startObject(uint32_t size) override;
	void endObject(void) override;

	void writeKey(const char* key) override;

	void writeInt8(const char* key, int8_t value) override;
	void writeInt16(const char* key, int16_t value) override;
	void writeInt32(const char* key, int32_t value) override;
	void writeInt64(const char* key, int64_t value) override;

	void writeUInt8(const char* key, uint8_t value) override;
	void writeUInt16(const char* key, uint16_t value) override;
	void writeUInt32(const char* key, uint32_t value) override;
	void writeUInt64(const char* key, uint64_t value) override;

	void writeFloat(const char* key, float value) override;
	void writeDouble(const char* key, double value) override;

	void writeString(const char* key, const char* value) override;

	void writeInt8(int8_t value) override;
	void writeInt16(int16_t value) override;
	void writeInt32(int32_t value) override;
	void writeInt64(int64_t value) override;

	void writeUInt8(uint8_t value) override;
	void writeUInt16(uint16_t value) override;
	void writeUInt32(uint32_t value) override;
	void writeUInt64(uint64_t value) override;

	void writeFloat(float value) override;
	void writeDouble(double value) override;

	void writeString(const char* value) override;

private:
	MessagePackWriter _writer;
};

NS_END
