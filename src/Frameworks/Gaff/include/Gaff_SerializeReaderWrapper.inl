/************************************************************************************
Copyright (C) 2019 by Nicholas LaCroix

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
SerializeReaderWrapper<Allocator>::SerializeReaderWrapper(const Allocator& allocator):
	_allocator(allocator)
{
}

template <class Allocator>
SerializeReaderWrapper<Allocator>::~SerializeReaderWrapper(void)
{
	if (_reader) {
		Deconstruct(_reader);
	}
}

template <class Allocator>
bool SerializeReaderWrapper<Allocator>::parseMPack(const char* buffer, size_t size)
{
	if (!_mpack_reader.parse(buffer, size)) {
		return false;
	}

	Construct(&_mpack, _mpack_reader.getRoot(), _allocator);
	_reader = &_mpack;

	return true;
}

template <class Allocator>
bool SerializeReaderWrapper<Allocator>::parseJSON(const char* buffer)
{
	Gaff::JSON json;

	if (!json.parse(buffer)) {
		return false;
	}

	Construct(&_json, json, _allocator);
	_reader = &_json;

	return true;
}

template <class Allocator>
const ISerializeReader* SerializeReaderWrapper<Allocator>::getReader(void) const
{
	return _reader;
}
