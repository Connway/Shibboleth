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

#include "Shibboleth_IFileSystem.h"
#include "Shibboleth_String.h"
#include "Shibboleth_Vector.h"
#include <Gaff_SpinLock.h>

NS_SHIBBOLETH

class LooseFile : public IFile
{
public:
	LooseFile(void);
	~LooseFile(void);

	size_t size(void) const;

	const char* getBuffer(void) const;
	char* getBuffer(void);

private:
	char* _file_buffer;
	size_t _file_size;

	friend class LooseFileSystem;
};

class LooseFileSystem : public IFileSystem
{
public:
	LooseFileSystem(void);
	~LooseFileSystem(void);

	IFile* openFile(const char* file_name);
	void closeFile(IFile* file);

	bool forEachFile(const char* directory, Gaff::FunctionBinder<bool, const char*, IFile*>& callback);

private:
	struct FileData
	{
		FileData(void) = default;

		FileData(const FileData& rhs):
			name(rhs.name),
			file(rhs.file),
			count(static_cast<int32_t>(rhs.count))
		{
		}

		FileData(FileData&& rhs):
			name(std::move(rhs.name)),
			file(rhs.file),
			count(static_cast<int32_t>(rhs.count))
		{
		}

		FileData& operator=(const FileData& rhs)
		{
			name = rhs.name;
			file = rhs.file;
			count = static_cast<int32_t>(rhs.count);
			return *this;
		}

		FileData& operator=(FileData&& rhs)
		{
			name = std::move(rhs.name);
			file = rhs.file;
			count = static_cast<int32_t>(rhs.count);
			return *this;
		}

		U8String name;
		IFile* file;
		std::atomic_int32_t count;
	};

	Vector<FileData> _files;
	Gaff::SpinLock _file_lock;
};

NS_END
