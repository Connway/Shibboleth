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

#include "Gaff_GUID.h"

#ifdef PLATFORM_WINDOWS
	#include "Gaff_Assert.h"
	#include <objbase.h>
#elif defined(PLATFORM_LINUX) || defined(PLATFORM_MAC)
	#include <uuid/uuid.h>
#endif

NS_GAFF

GUID GUID::Generate()
{
	GUID result;

#ifdef PLATFORM_WINDOWS
	GAFF_ASSERT(CoCreateGuid(reinterpret_cast<::GUID*>(&result)) == S_OK);

#elif defined(PLATFORM_LINUX) || defined(PLATFORM_MAC)
	uuid_t uuid;
	uuid_generate(uuid);

	result.byte_0 = uuid[0];
	result.byte_1 = uuid[1];
	result.byte_2 = uuid[2];
	result.byte_3 = uuid[3];
#endif

	return result;
}

bool GUID::IsValid() const
{
	return byte_0 != 0 || byte_1 != 0 || byte_2 != 0 || byte_3 != 0;
}

bool GUID::operator==(const GUID& rhs) const
{
	return byte_0 == rhs.byte_0 && byte_1 == rhs.byte_1 && byte_2 == rhs.byte_2 && byte_3 == rhs.byte_3;
}

bool GUID::operator!=(const GUID& rhs) const
{
	return byte_0 != rhs.byte_0 || byte_1 != rhs.byte_1 || byte_2 != rhs.byte_2 || byte_3 != rhs.byte_3;
}

NS_END
