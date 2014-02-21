/************************************************************************************
Copyright (C) 2013 by Nicholas LaCroix

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

#if defined(__linux__) || defined(__APPLE__)

#include "Gaff_CriticalSection_Linux.h"

NS_GAFF

// to make GCC stop complaining
static pthread_mutex_t MUTEX_NULL = PTHREAD_MUTEX_INITIALIZER;

CriticalSection::CriticalSection(void): _cs(MUTEX_NULL) {}

CriticalSection::~CriticalSection(void)
{
}

void CriticalSection::lock(void) const
{
	pthread_mutex_lock(&_cs);
}

bool CriticalSection::tryLock(void) const
{
	return pthread_mutex_trylock(&_cs) == 0;
}


void CriticalSection::unlock(void) const
{
	pthread_mutex_unlock(&_cs);
}

NS_END

#endif
