/************************************************************************************
Copyright (C) 2016 by Nicholas LaCroix

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

#include <Shibboleth_Defines.h>
#include <stdint.h>

NS_SHIBBOLETH

class UserDataWrapper
{
public:

	UserDataWrapper(uint64_t user_data = 0):
		_user_data(user_data)
	{
	}

	~UserDataWrapper(void)
	{
	}

	template <class T>
	const T& getUserData(void) const
	{
		static_GAFF_ASSERT(sizeof(T) <= sizeof(uint64_t), "UserDataWrapper: sizeof(T) is greater than 64-bits!");
		return reinterpret_cast<T>(_user_data);
	}

	template <class T>
	T& getUserData(void)
	{
		static_GAFF_ASSERT(sizeof(T) <= sizeof(uint64_t), "UserDataWrapper: sizeof(T) is greater than 64-bits!");
		return reinterpret_cast<T>(_user_data);
	}

	uint64_t get(void) const
	{
		return _user_data;
	}

private:
	uint64_t _user_data;
};

NS_END
