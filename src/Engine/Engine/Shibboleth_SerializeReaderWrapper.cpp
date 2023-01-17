/************************************************************************************
Copyright (C) 2022 by Nicholas LaCroix

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

#include "Shibboleth_SerializeReaderWrapper.h"

NS_SHIBBOLETH

SerializeReaderWrapper::SerializeReaderWrapper(const ProxyAllocator& allocator):
	_allocator(allocator)
{
}

SerializeReaderWrapper::SerializeReaderWrapper(SerializeReaderWrapper&& reader):
	_allocator(reader._allocator),
	_reader(nullptr),
	_mpack_reader(std::move(reader._mpack_reader)),
	_error_text(reader._error_text),
	_is_mpack(reader._is_mpack)
{
	if (_is_mpack) {
		_mpack = std::move(reader._mpack);
		_reader = &_mpack;
	} else {
		_json = std::move(reader._json);
		_reader = &_json;
	}
}

SerializeReaderWrapper::~SerializeReaderWrapper(void)
{
	if (_reader) {
		Gaff::Deconstruct(_reader);
	}
}

SerializeReaderWrapper& SerializeReaderWrapper::operator=(SerializeReaderWrapper&& readerWrapper)
{
	if (_reader) {
		Gaff::Deconstruct(_reader);
	}

	_allocator = readerWrapper._allocator;
	_is_mpack = readerWrapper._is_mpack;
	_error_text = readerWrapper._error_text;

	if (readerWrapper._reader) {
		if (_is_mpack) {
			_mpack_reader = std::move(readerWrapper._mpack_reader);

			Construct(&_mpack, _mpack_reader.getRoot(), _allocator);
			_reader = &_mpack;

		} else {
			Construct(&_json, std::move(readerWrapper._json));
			_reader = &_json;
		}
	}

	return *this;
}

bool SerializeReaderWrapper::parseMPack(const char* buffer, size_t size, bool take_ownership)
{
	if (!_mpack_reader.parse(buffer, size, take_ownership)) {
		_error_text = _mpack_reader.getErrorText();
		return false;
	}

	Construct(&_mpack, _mpack_reader.getRoot(), _allocator);
	_reader = &_mpack;
	_is_mpack = true;

	return true;
}

bool SerializeReaderWrapper::parseJSON(const char8_t* buffer, const char8_t* schema)
{
	Gaff::JSON json;

	const bool success = (schema) ? json.parse(buffer, schema) : json.parse(buffer);

	if (!success) {
		_error_text = reinterpret_cast<const char*>(json.getErrorText());
		return false;
	}

	Construct(&_json, std::move(json), _allocator);
	_reader = &_json;

	return true;
}

const ISerializeReader* SerializeReaderWrapper::getReader(void) const
{
	return _reader;
}

const char* SerializeReaderWrapper::getErrorText(void) const
{
	return _error_text;
}

void SerializeReaderWrapper::freeReader(void)
{
	if (_reader) {
		Gaff::Deconstruct(_reader);
		_reader = nullptr;
	}
}

NS_END
