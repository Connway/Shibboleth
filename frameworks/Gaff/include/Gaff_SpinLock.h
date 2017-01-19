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

#include "Gaff_Defines.h"
#include <atomic>

NS_GAFF

class SpinLock
{
public:
	SpinLock(void);

	INLINE void lock(void);
	INLINE bool tryLock(void);
	INLINE void unlock(void);

private:
	std::atomic_flag _lock = { 0 };

	GAFF_NO_MOVE(SpinLock);
	GAFF_NO_COPY(SpinLock);
};

class ReadWriteSpinLock
{
public:
	ReadWriteSpinLock(void);

	INLINE void readLock(void);
	INLINE bool tryReadLock(void);
	INLINE void readUnlock(void);

	INLINE void writeLock(void);
	INLINE bool tryWriteLock(void);
	INLINE void writeUnlock(void);

private:
	std::atomic_int32_t _read_lock = 0;
	std::atomic_flag _write_lock = { 0 };

	GAFF_NO_MOVE(ReadWriteSpinLock);
	GAFF_NO_COPY(ReadWriteSpinLock);
};

NS_END
