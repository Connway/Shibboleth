/************************************************************************************
Copyright (C) 2020 by Nicholas LaCroix

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

#include "Gaff_SerializeReader.h"
#include "Gaff_MessagePack.h"
#include "Gaff_JSON.h"

NS_GAFF

template <class Allocator>
class SerializeReaderWrapper final
{
public:
	SerializeReaderWrapper(const Allocator& allocator = Allocator());
	SerializeReaderWrapper(SerializeReaderWrapper&&) = default;
	~SerializeReaderWrapper(void);

	SerializeReaderWrapper& operator=(SerializeReaderWrapper&&);

	bool parseMPack(const char* buffer, size_t size, bool take_ownership = false);
	bool parseJSON(const char* buffer);

	const ISerializeReader* getReader(void) const;
	const char* getErrorText(void) const;

private:
	Allocator _allocator;

	ISerializeReader* _reader = nullptr;

	MessagePackReader _mpack_reader;

	union
	{
		SerializeReader<MessagePackNode, Allocator> _mpack;
		SerializeReader<JSON, Allocator> _json;
	};

	const char* _error_text = nullptr;
	bool _is_mpack = false;
};

#include "Gaff_SerializeReaderWrapper.inl"

NS_END
