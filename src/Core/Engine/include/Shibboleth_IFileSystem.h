/************************************************************************************
Copyright (C) 2023 by Nicholas LaCroix

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

#include "Shibboleth_Defines.h"
#include <EASTL/functional.h>

NS_SHIBBOLETH

class IFile
{
public:
	IFile(void) {}
	virtual ~IFile(void) {}

	// Only used for files opened for read
	virtual size_t size(void) const = 0;

	virtual const int8_t* getBuffer(void) const = 0;
	virtual int8_t* getBuffer(void) = 0;

	//virtual void write(const char* buffer, unsigned int buffer_size) = 0;
};

class IFileSystem
{
public:
	//enum OpenMode { OT_READ = 0, OT_WRITE };

	IFileSystem(void) {}
	virtual ~IFileSystem(void) {}

	virtual IFile* openFile(const char8_t* file_name/*, OpenMode mode*/) = 0;
	virtual void closeFile(const IFile* file) = 0;

	// This function circumvents the file cache. If a file is already open, it will open it again and allocate another buffer.
	// Should be used during initialization/loading phases only.
	virtual bool forEachFile(const char8_t* directory, eastl::function<bool (const char8_t*, IFile*)>& callback, const char8_t* extension, bool recursive = false) = 0;
	virtual bool forEachFile(const char8_t* directory, eastl::function<bool (const char8_t*, IFile*)>& callback, bool recursive = false) = 0;
};

NS_END
