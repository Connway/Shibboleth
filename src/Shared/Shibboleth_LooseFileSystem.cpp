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

#include "Shibboleth_LooseFileSystem.h"
#include "Shibboleth_String.h"
#include <Gaff_ScopedLock.h>
#include <Gaff_Atomic.h>
#include <Gaff_Utils.h>
#include <Gaff_File.h>

NS_SHIBBOLETH

LooseFile::LooseFile(void)
{
}

LooseFile::~LooseFile(void)
{
	if (_file_buffer) {
		GetAllocator()->free(_file_buffer);
	}
}

size_t LooseFile::size(void) const
{
	return _file_size;
}

const char* LooseFile::getBuffer(void) const
{
	return _file_buffer;
}

char* LooseFile::getBuffer(void)
{
	return _file_buffer;
}


LooseFileSystem::LooseFileSystem(void)
{
}

LooseFileSystem::~LooseFileSystem(void)
{
	for (auto it = _files.begin(); it != _files.end(); ++it) {
		GetAllocator()->freeT(it->file);
	}
}

IFile* LooseFileSystem::openFile(const char* file_name)
{
	GAFF_ASSERT(file_name && strlen(file_name));
	Gaff::ScopedLock<Gaff::SpinLock> scoped_lock(_file_lock);

	auto it = _files.linearSearch(file_name, [](const FileData& lhs, const char* rhs) -> bool
	{
		return lhs.name == rhs;
	});

	if (it == _files.end()) {
		AString name = AString("./") + file_name; // Pre-pend './' to name
		Gaff::File loose_file;

		if (!loose_file.open(name.getBuffer(), Gaff::File::READ_BINARY)) {
			return nullptr;
		}

		LooseFile* file = GetAllocator()->template allocT<LooseFile>();

		// Should probably log that the allocation failed
		if (!file) {
			return nullptr;
		}

		file->_file_size= loose_file.getFileSize();
		file->_file_buffer = reinterpret_cast<char*>(GetAllocator()->alloc(file->_file_size));

		if (!file->_file_buffer) {
			GetAllocator()->freeT(file);
			return nullptr;
		}

		if (!loose_file.readEntireFile(file->_file_buffer)) {
			GetAllocator()->freeT(file);
			return nullptr;
		}

		FileData file_data;
		file_data.name = file_name;
		file_data.file = file;
		file_data.count = 1;

		_files.push(std::move(file_data));
		return file;

	} else {
		AtomicIncrement(&it->count);
		return it->file;
	}
}

void LooseFileSystem::closeFile(IFile* file)
{
	GAFF_ASSERT(file);
	Gaff::ScopedLock<Gaff::SpinLock> scoped_lock(_file_lock);

	auto it = _files.linearSearch(file, [](const FileData& lhs, const IFile* rhs) -> bool
	{
		return lhs.file == rhs;
	});

	if (it != _files.end()) {
		unsigned int new_count = AtomicDecrement(&it->count);

		if (!new_count) {
			GetAllocator()->freeT(it->file);
			_files.fastErase(it);
		}
	}
}

bool LooseFileSystem::forEachFile(const char* directory, const Gaff::FunctionBinder<bool, const char*, IFile*>& callback)
{
	return Gaff::ForEachTypeInDirectory<Gaff::FDT_RegularFile>(directory, [&](const char* file_name, size_t) -> bool
	{
		AString full_path(directory);

		if (full_path[full_path.size() - 1] != '/') {
			full_path += '/';
		}

		full_path += file_name;

		IFile* file = openFile(full_path.getBuffer());

		//if (!file) {
		//	// handle failure
		//}

		return callback(file_name, file);
	});
}

NS_END
