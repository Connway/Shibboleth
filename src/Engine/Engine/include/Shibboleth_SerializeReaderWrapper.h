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

#pragma once

#include "Shibboleth_SerializeReader.h"
#include <Gaff_MessagePack.h>
#include <Gaff_JSON.h>

NS_SHIBBOLETH

class SerializeReaderWrapper final
{
public:
	SerializeReaderWrapper(const ProxyAllocator& allocator = ProxyAllocator());
	SerializeReaderWrapper(SerializeReaderWrapper&&);
	~SerializeReaderWrapper(void);

	SerializeReaderWrapper& operator=(SerializeReaderWrapper&&);

	bool parseMPack(const char* buffer, size_t size, bool take_ownership = false);
	bool parseJSON(const char8_t* buffer, const char8_t* schema = nullptr);

	const ISerializeReader* getReader(void) const;
	const char* getErrorText(void) const;

	void freeReader(void);

private:
	ProxyAllocator _allocator;

	ISerializeReader* _reader = nullptr;

	Gaff::MessagePackReader _mpack_reader;

	union
	{
		SerializeReader<Gaff::MessagePackNode> _mpack;
		SerializeReader<Gaff::JSON> _json;
	};

	const char* _error_text = nullptr;
	bool _is_mpack = false;
};

NS_END
