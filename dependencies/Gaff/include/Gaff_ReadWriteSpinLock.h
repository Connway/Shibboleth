/************************************************************************************
Copyright (C) 2015 by Nicholas LaCroix

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

/*! \file */

#pragma once

#include "Gaff_Defines.h"

NS_GAFF

/*!
	\brief A spin lock that manages read/write access.
*/
class ReadWriteSpinLock
{
public:
	ReadWriteSpinLock(ReadWriteSpinLock&&);
	ReadWriteSpinLock(void);
	~ReadWriteSpinLock(void);

	const ReadWriteSpinLock& operator=(ReadWriteSpinLock&&);

	void readLock(void) const;
	void readUnlock(void) const;

	void writeLock(void) const;
	void writeUnlock(void) const;

private:
	mutable volatile long _write_lock;
	mutable volatile long _read_lock;

	GAFF_NO_COPY(ReadWriteSpinLock);
};

NS_END
