/************************************************************************************
Copyright (C) 2018 by Nicholas LaCroix

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

#include "Gaff_SpinLock.h"
#include "Gaff_Assert.h"

#ifdef PLATFORM_WINDOWS
	#include "Gaff_IncludeWindows.h"
#endif

NS_GAFF

SpinLock::SpinLock(void)
{
}

void SpinLock::lock(void)
{
	int32_t tries = 0;

	while (_lock.test_and_set()) {
		++tries;

		if (tries == NUM_TRIES_UNTIL_YIELD) {
			tries = 0;
			YieldThread();
		}
	}
}

bool SpinLock::tryLock(void)
{
	return !_lock.test_and_set();
}

void SpinLock::unlock(void)
{
	_lock.clear();
}



ReadWriteSpinLock::ReadWriteSpinLock(void)
{
}

void ReadWriteSpinLock::readLock(void)
{
	int32_t tries = 0;

	while (!_read_lock && _write_lock.test_and_set()) {
		++tries;

		if (tries == NUM_TRIES_UNTIL_YIELD) {
			tries = 0;
			YieldThread();
		}
	}

	++_read_lock;
}

bool ReadWriteSpinLock::tryReadLock(void)
{
	if (!_write_lock.test_and_set()) {
		++_read_lock;
		return true;
	}

	return false;
}

void ReadWriteSpinLock::readUnlock(void)
{
	int32_t new_val = --_read_lock;
	GAFF_ASSERT(new_val > -1);

	if (!new_val) {
		_write_lock.clear();
	}
}

void ReadWriteSpinLock::writeLock(void)
{
	int32_t tries = 0;

	while (_write_lock.test_and_set()) {
		++tries;

		if (tries == NUM_TRIES_UNTIL_YIELD) {
			tries = 0;
			YieldThread();
		}
	}
}

bool ReadWriteSpinLock::tryWriteLock(void)
{
	return !_write_lock.test_and_set();
}

void ReadWriteSpinLock::writeUnlock(void)
{
	_write_lock.clear();
}

NS_END
