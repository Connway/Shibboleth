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

#include "Shibboleth_HashString.h"
#include "Shibboleth_HashMap.h"
#include "Shibboleth_String.h"
#include <Gaff_SpinLock.h>
#include <Gaff_File.h>
#include <Gaff_Pair.h>

NS_SHIBBOLETH

class LogManager
{
public:
	typedef Gaff::Pair<Gaff::File, Gaff::SpinLock*> FileLockPair;

	LogManager(Allocator& allocator);
	~LogManager(void);

	void destroy(void);

	bool openLogFile(const AHashString& filename);
	INLINE bool openLogFile(const AString& filename);
	INLINE bool openLogFile(const char* filename);
	INLINE void closeLogFile(const AHashString& filename);
	INLINE void closeLogFile(const AString& filename);
	INLINE void closeLogFile(const char* filename);
	INLINE FileLockPair& getLogFile(const AHashString& filename);
	INLINE FileLockPair& getLogFile(const AString& filename);
	INLINE FileLockPair& getLogFile(const char* filename);

private:
	HashMap< AHashString, Gaff::Pair<Gaff::File, Gaff::SpinLock*> > _files;
	Allocator& _allocator;

	GAFF_NO_COPY(LogManager);
	GAFF_NO_MOVE(LogManager);
};

NS_END
