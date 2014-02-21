/************************************************************************************
Copyright (C) 2014 by Nicholas LaCroix

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

#include "Gaff_Defines.h"

NS_GAFF

template <class Lock>
class ScopedLock
{
public:
	ScopedLock(const Lock& lock):
		_lock(lock)
	{
		_lock.lock();
	}

	~ScopedLock(void)
	{
		_lock.unlock();
	}

private:
	const Lock& _lock;

	GAFF_NO_COPY(ScopedLock);
	GAFF_NO_MOVE(ScopedLock);
};

template <class Lock>
class ScopedReadLock
{
public:
	ScopedReadLock(const Lock& lock):
		_lock(lock)
	{
		_lock.readLock();
	}

	~ScopedReadLock(void)
	{
		_lock.readUnlock();
	}

private:
	const Lock& _lock;

	GAFF_NO_COPY(ScopedReadLock);
	GAFF_NO_MOVE(ScopedReadLock);
};

template <class Lock>
class ScopedWriteLock
{
public:
	ScopedWriteLock(const Lock& lock):
		_lock(lock)
	{
		_lock.writeLock();
	}

	~ScopedWriteLock(void)
	{
		_lock.writeUnlock();
	}

private:
	const Lock& _lock;

	GAFF_NO_COPY(ScopedWriteLock);
	GAFF_NO_MOVE(ScopedWriteLock);
};

NS_END
