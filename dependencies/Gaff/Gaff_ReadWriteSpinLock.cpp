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

#include "Gaff_ReadWriteSpinLock.h"
#include "Gaff_Atomic.h"

NS_GAFF

ReadWriteSpinLock::ReadWriteSpinLock(ReadWriteSpinLock&& rwsl):
	_write_lock(0), _read_lock(0)
{
}

ReadWriteSpinLock::ReadWriteSpinLock(void):
	_write_lock(0), _read_lock(0)
{
}

ReadWriteSpinLock::~ReadWriteSpinLock(void)
{
}

const ReadWriteSpinLock& ReadWriteSpinLock::operator=(ReadWriteSpinLock&& rhs)
{
	return *this;
}

void ReadWriteSpinLock::readLock(void) const
{
	AtomicIncrement(&_read_lock);

	while (_write_lock) {
	}
}

void ReadWriteSpinLock::readUnlock(void) const
{
	AtomicDecrement(&_read_lock);
}

void ReadWriteSpinLock::writeLock(void) const
{
	while (AtomicAcquire(&_write_lock)) {
		while (_write_lock) {
		}
	}

	while (_read_lock) {
	}
}

void ReadWriteSpinLock::writeUnlock(void) const
{
	AtomicRelease(&_write_lock);
}

NS_END
