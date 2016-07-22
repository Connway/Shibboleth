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

#include "Gaff_ReadWriteSpinLock.h"
#include "Gaff_Assert.h"
#include "Gaff_Atomic.h"

NS_GAFF

ReadWriteSpinLock::ReadWriteSpinLock(const ReadWriteSpinLock& lock):
	_write_lock(0), _read_lock(0)
{
	GAFF_ASSERT(!lock._write_lock && !lock._read_lock);
}

ReadWriteSpinLock::ReadWriteSpinLock(void):
	_write_lock(0), _read_lock(0)
{
}

ReadWriteSpinLock::~ReadWriteSpinLock(void)
{
}

/*!
	\brief I think I added this as a hack for something, cause it doesn't do anything.
*/
const ReadWriteSpinLock& ReadWriteSpinLock::operator=(const ReadWriteSpinLock& rhs)
{
	GAFF_ASSERT(!rhs._write_lock && !rhs._read_lock);
	return *this;
}

/*!
	\brief Acquires a lock for read access. If the lock for write is already acquired, blocks until it is released.
*/
void ReadWriteSpinLock::readLock(void) const
{
	unsigned int tries = 0;

	/*
		We wait for the write lock to be released first
		to avoid situations where someone requests to write,
		but read requests come in too fast and never reaches
		a read count of zero, causing the writing thread to
		become frozen indefinitely.
	*/
	while (_write_lock) {
		++tries;

		if (tries == NUM_TRIES_UNTIL_YIELD) {
			tries = 0;
			YieldThread();
		}
	}

	AtomicIncrement(&_read_lock);

	tries = 0;

	// Double check just in case.
	while (_write_lock) {
		++tries;

		if (tries == NUM_TRIES_UNTIL_YIELD) {
			tries = 0;
			YieldThread();
		}
	}
}

/*!
	\brief Releases a read lock.
*/
void ReadWriteSpinLock::readUnlock(void) const
{
	GAFF_ASSERT(_read_lock);
	AtomicDecrement(&_read_lock);
}

/*!
	\brief Acquires a lock for write access. If a lock for read or write is already acquired, blocks until they are released.
*/
void ReadWriteSpinLock::writeLock(void) const
{
	unsigned int tries = 0;

	while (AtomicAcquire(&_write_lock)) {
		++tries;

		if (tries == NUM_TRIES_UNTIL_YIELD) {
			tries = 0;
			YieldThread();
		}
	}

	tries = 0;

	while (_read_lock) {
		++tries;

		if (tries == NUM_TRIES_UNTIL_YIELD) {
			tries = 0;
			YieldThread();
		}
	}
}

/*!
	\brief Releases a write lock.
*/
void ReadWriteSpinLock::writeUnlock(void) const
{
	GAFF_ASSERT(_write_lock);
	AtomicRelease(&_write_lock);
}

NS_END
