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

#pragma once

#include "Shibboleth_IFileSystem.h"
#include "Shibboleth_String.h"
#include "Shibboleth_Array.h"
#include <Gaff_SpinLock.h>
#include <unzip.h>

NS_SHIBBOLETH

class PackFile : public IFile
{
public:
	PackFile(void);
	~PackFile(void);

	size_t size(void) const;

	const char* getBuffer(void) const;
	char* getBuffer(void);

private:
	char* _file_buffer;
	size_t _file_size;

	friend class PackFileSystem;
};

class PackFileSystem : IFileSystem
{
public:
	PackFileSystem(void);
	~PackFileSystem(void);

	bool addPackFile(const char* file_name);

	IFile* openFile(const char* file_name);
	void closeFile(IFile* file);

	bool forEachFile(const char* directory, const Gaff::FunctionBinder<bool, const char*, IFile*>& callback);

private:
	struct FileData
	{
		FileData(void) {}
		FileData(FileData&& file_data):
			name(std::move(file_data.name)),
			file(file_data.file),
			count(file_data.count)
		{
		}

		AString name;
		IFile* file;
		volatile unsigned int count;
	};

	Array<unzFile> _pack_files;
	Array<FileData> _files;
	Gaff::SpinLock _file_lock;
};

NS_END
