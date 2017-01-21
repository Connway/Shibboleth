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

NS_GAFF

class ISerializeReader
{
public:
	virtual ~ISerializeReader(void) {}

	virtual int8_t readInt8(void) const = 0;
	virtual int16_t readInt16(void) const = 0;
	virtual int32_t readInt32(void) const = 0;
	virtual int64_t readInt64(void) const = 0;

	virtual uint8_t readUInt8(void) const = 0;
	virtual uint16_t readUInt16(void) const = 0;
	virtual uint32_t readUInt32(void) const = 0;
	virtual uint64_t readUInt64(void) const = 0;

	virtual float readFloat(void) const = 0;
	virtual double readDouble(void) const = 0;

	virtual size_t readStringLength(void) const = 0;
	virtual const char* readString(void) const = 0;
};

class ISerializeWriter
{
public:
	virtual ~ISerializeWriter(void) {}

	virtual void writeInt8(int8_t value) const = 0;
	virtual void writeInt16(int16_t value) const = 0;
	virtual void writeInt32(int32_t value) const = 0;
	virtual void writeInt64(int64_t value) const = 0;

	virtual void writeUInt8(uint8_t value) const = 0;
	virtual void writeUInt16(uint16_t value) const = 0;
	virtual void writeUInt32(uint32_t value) const = 0;
	virtual void writeUInt64(uint64_t value) const = 0;

	virtual void writeFloat(float value) const = 0;
	virtual void writeDouble(double value) const = 0;

	virtual void writeString(const char* value, size_t size) const = 0;
	virtual void writeString(const char* value) const = 0;
};

NS_END
