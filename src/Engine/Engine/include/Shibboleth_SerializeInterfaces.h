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

#pragma once

#include "Shibboleth_Defines.h"
#include <Gaff_Assert.h>

NS_SHIBBOLETH

class ScopeGuard;

class ISerializeReader
{
public:
	template <class Callback>
	bool forEachInObject(Callback&& callback) const
	{
		GAFF_ASSERT(isObject());
		const int32_t len = size();
		char key[256] = {};

		for (int32_t i = 0; i < len; ++i) {
			getKey(key, ARRAY_SIZE(key), i);
			enterElement(key);

			if (callback(key)) {
				exitElement();
				return true;
			}

			exitElement();
		}

		return false;
	}

	template <class Callback>
	bool forEachInArray(Callback&& callback) const
	{
		GAFF_ASSERT(isArray());
		const int32_t len = size();

		for (int32_t i = 0; i < len; ++i) {
			enterElement(i);

			if (callback(i)) {
				exitElement();
				return true;
			}

			exitElement();
		}

		return false;
	}


	virtual ~ISerializeReader(void) {}

	virtual bool isObject(void) const = 0;
	virtual bool isArray(void) const = 0;
	virtual bool isString(void) const = 0;
	virtual bool isNumber(void) const = 0;
	virtual bool isInt8(void) const = 0;
	virtual bool isUInt8(void) const = 0;
	virtual bool isInt16(void) const = 0;
	virtual bool isUInt16(void) const = 0;
	virtual bool isInt32(void) const = 0;
	virtual bool isUInt32(void) const = 0;
	virtual bool isInt64(void) const = 0;
	virtual bool isUInt64(void) const = 0;
	virtual bool isFloat(void) const = 0;
	virtual bool isDouble(void) const = 0;
	virtual bool isBool(void) const = 0;
	virtual bool isTrue(void) const = 0;
	virtual bool isFalse(void) const = 0;
	virtual bool isNull(void) const = 0;

	virtual void enterElement(const char* key) const = 0;
	virtual void enterElement(int32_t index)  const = 0;
	virtual void exitElement(void)  const = 0;

	virtual ScopeGuard enterElementGuard(const char* key) const = 0;
	virtual ScopeGuard enterElementGuard(int32_t index) const = 0;

	virtual void freeString(const char* str) const = 0;
	virtual int32_t size(void) const = 0;

	virtual bool exists(const char* key) const = 0;

	virtual const char* getKey(char* buffer, size_t buf_size, int32_t index) const = 0;
	virtual const char* getKey(int32_t index) const = 0;

	virtual const char* readString(char* buffer, size_t buf_size, const char* default_value) const = 0;
	virtual const char* readString(const char* default_value) const = 0;
	virtual int8_t readInt8(int8_t default_value) const = 0;
	virtual uint8_t readUInt8(uint8_t default_value) const = 0;
	virtual int16_t readInt16(int16_t default_value) const = 0;
	virtual uint16_t readUInt16(uint16_t default_value) const = 0;
	virtual int32_t readInt32(int32_t default_value) const = 0;
	virtual uint32_t readUInt32(uint32_t default_value) const = 0;
	virtual int64_t readInt64(int64_t default_value) const = 0;
	virtual uint64_t readUInt64(uint64_t default_value) const = 0;
	virtual float readFloat(float default_value) const = 0;
	virtual double readDouble(double default_value) const = 0;
	virtual double readNumber(double default_value) const = 0;
	virtual bool readBool(bool default_value) const = 0;

	virtual const char* readString(char* buffer, size_t buf_size) const = 0;
	virtual const char* readString(void) const = 0;
	virtual int8_t readInt8(void) const = 0;
	virtual uint8_t readUInt8(void) const = 0;
	virtual int16_t readInt16(void) const = 0;
	virtual uint16_t readUInt16(void) const = 0;
	virtual int32_t readInt32(void) const = 0;
	virtual uint32_t readUInt32(void) const = 0;
	virtual int64_t readInt64(void) const = 0;
	virtual uint64_t readUInt64(void) const = 0;
	virtual float readFloat(void) const = 0;
	virtual double readDouble(void) const = 0;
	virtual double readNumber(void) const = 0;
	virtual bool readBool(void) const = 0;

	//virtual const char* readString(const char* key, char* buffer, size_t buf_size, const char* default_value) const = 0;
	//virtual const char* readString(const char* key, const char* default_value) const = 0;
	virtual int8_t readInt8(const char* key, int8_t default_value) const = 0;
	virtual uint8_t readUInt8(const char* key, uint8_t default_value) const = 0;
	virtual int16_t readInt16(const char* key, int16_t default_value) const = 0;
	virtual uint16_t readUInt16(const char* key, uint16_t default_value) const = 0;
	virtual int32_t readInt32(const char* key, int32_t default_value) const = 0;
	virtual uint32_t readUInt32(const char* key, uint32_t default_value) const = 0;
	virtual int64_t readInt64(const char* key, int64_t default_value) const = 0;
	virtual uint64_t readUInt64(const char* key, uint64_t default_value) const = 0;
	virtual float readFloat(const char* key, float default_value) const = 0;
	virtual double readDouble(const char* key, double default_value) const = 0;
	virtual double readNumber(const char* key, double default_value) const = 0;
	virtual bool readBool(const char* key, bool default_value) const = 0;

};

class ISerializeWriter
{
public:
	virtual ~ISerializeWriter(void) {}

	virtual void startArray(uint32_t size) = 0;
	virtual void endArray(void) = 0;

	virtual void startObject(uint32_t size) = 0;
	virtual void endObject(void) = 0;

	virtual void writeKey(const char* key) = 0;

	virtual void writeInt8(const char* key, int8_t value) = 0;
	virtual void writeInt16(const char* key, int16_t value) = 0;
	virtual void writeInt32(const char* key, int32_t value) = 0;
	virtual void writeInt64(const char* key, int64_t value) = 0;

	virtual void writeUInt8(const char* key, uint8_t value) = 0;
	virtual void writeUInt16(const char* key, uint16_t value) = 0;
	virtual void writeUInt32(const char* key, uint32_t value) = 0;
	virtual void writeUInt64(const char* key, uint64_t value) = 0;

	virtual void writeFloat(const char* key, float value) = 0;
	virtual void writeDouble(const char* key, double value) = 0;
	virtual void writeBool(const char* key, bool value) = 0;

	virtual void writeString(const char* key, const char* value) = 0;

	virtual void writeInt8(int8_t value) = 0;
	virtual void writeInt16(int16_t value) = 0;
	virtual void writeInt32(int32_t value) = 0;
	virtual void writeInt64(int64_t value) = 0;

	virtual void writeUInt8(uint8_t value) = 0;
	virtual void writeUInt16(uint16_t value) = 0;
	virtual void writeUInt32(uint32_t value) = 0;
	virtual void writeUInt64(uint64_t value) = 0;

	virtual void writeFloat(float value) = 0;
	virtual void writeDouble(double value) = 0;
	virtual void writeBool(bool value) = 0;

	virtual void writeString(const char* value) = 0;
};

class ScopeGuard
{
public:
	GAFF_NO_COPY(ScopeGuard);

	ScopeGuard(const ISerializeReader& reader, const char* key): _reader(&reader) { _reader->enterElement(key); }
	ScopeGuard(const ISerializeReader& reader, int32_t index): _reader(&reader) { _reader->enterElement(index); }
	~ScopeGuard(void) { if (_reader) _reader->exitElement(); }

	ScopeGuard(ScopeGuard&& guard): _reader(guard._reader) { guard._reader = nullptr; }

	ScopeGuard& operator=(ScopeGuard&& rhs)
	{
		_reader = rhs._reader;
		rhs._reader = nullptr;
		return *this;
	}

private:
	const ISerializeReader* _reader = nullptr;
};

NS_END
