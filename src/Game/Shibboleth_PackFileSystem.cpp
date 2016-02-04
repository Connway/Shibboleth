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

#include "Shibboleth_PackFileSystem.h"
#include <Gaff_ScopedLock.h>
#include <Gaff_Atomic.h>

NS_SHIBBOLETH

PackFile::PackFile(void):
	_file_buffer(nullptr), _file_size(0)
{
}

PackFile::~PackFile(void)
{
	if (_file_buffer) {
		GetAllocator()->free(_file_buffer);
	}
}

size_t PackFile::size(void) const
{
	return _file_size;
}

const char* PackFile::getBuffer(void) const
{
	return _file_buffer;
}

char* PackFile::getBuffer(void)
{
	return _file_buffer;
}


PackFileSystem::PackFileSystem(void)
{
}

PackFileSystem::~PackFileSystem(void)
{
	for (auto it = _files.begin(); it != _files.end(); ++it) {
		GetAllocator()->freeT(it->file);
	}

	for (auto it = _pack_files.begin(); it != _pack_files.end(); ++it) {
		unzClose(*it);
	}
}

bool PackFileSystem::addPackFile(const char* file_name)
{
	// We are assuming we are not calling addPackFile() more than once on the same file.
	unzFile pack_file = unzOpen(file_name);

	if (!pack_file) {
		return false;
	}

	_pack_files.push(pack_file);
	return true;
}

IFile* PackFileSystem::openFile(const char* file_name)
{
	assert(file_name && strlen(file_name));
	Gaff::ScopedLock<Gaff::SpinLock> scoped_lock(_file_lock);

	auto it = _files.linearSearch(file_name, [](const FileData& lhs, const char* rhs) -> bool
	{
		return lhs.name == rhs;
	});

	if (it == _files.end()) {
		auto it_pack = _pack_files.begin();

		// We just take the first matching file. File paths should be unique to each packfile.
		for (; it_pack != _pack_files.end(); ++it_pack) {
			if (unzLocateFile(*it_pack, file_name, 1) == UNZ_OK) {
				break;
			}
		}

		if (it_pack == _pack_files.end()) {
			// Log error
			return nullptr;
		}

		if (unzOpenCurrentFile(*it_pack) != UNZ_OK) {
			// Log error
			return nullptr;
		}

		PackFile* file = GetAllocator()->template allocT<PackFile>();

		if (!file) {
			// Log error
			return nullptr;
		}

		unz_file_info file_info;

		if (unzGetCurrentFileInfo(*it_pack, &file_info, nullptr, 0, nullptr, 0, nullptr, 0) != UNZ_OK) {
			// Log error
			GetAllocator()->freeT(file);
			return nullptr;
		}

		file->_file_buffer = reinterpret_cast<char*>(GetAllocator()->alloc(file_info.uncompressed_size));
		file->_file_size = file_info.uncompressed_size;

		if (!file->_file_buffer) {
			// Log error
			GetAllocator()->freeT(file);
			return nullptr;
		}

		if (unzReadCurrentFile(*it_pack, file->_file_buffer, static_cast<unsigned int>(file->_file_size)) < 0) {
			// Log error
			GetAllocator()->freeT(file);
			return nullptr;
		}

		unzCloseCurrentFile(*it_pack);

		FileData file_data;
		file_data.name = file_name;
		file_data.file = file;
		file_data.count = 1;

		_files.emplacePush(std::move(file_data));
		return file;

	} else {
		AtomicIncrement(&it->count);
		return it->file;
	}
}

void PackFileSystem::closeFile(IFile* file)
{
	assert(file);
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

bool PackFileSystem::forEachFile(const char* directory, const Gaff::FunctionBinder<bool, const char*, IFile*>& callback)
{
	size_t dir_len = strlen(directory);
	unz_file_info file_info;
	bool early_out = false;
	char temp[256] = { 0 };

	for (auto it = _pack_files.begin(); it != _pack_files.end(); ++it) {
		if (unzGoToFirstFile(*it) != UNZ_OK) {
				// Log error
			continue;
		}

		while (!unzeof(*it)) {
			if (unzOpenCurrentFile(*it) != UNZ_OK) {
				// Log error
				break;
			}

			if (unzGetCurrentFileInfo(*it, &file_info, temp, sizeof(temp), nullptr, 0, nullptr, 0) != UNZ_OK) {
				// Log error
				break;
			}

			if (!strncmp(temp, directory, dir_len)) {
				PackFile* file = GetAllocator()->template allocT<PackFile>();

				if (!file) {
					// Log error
					break;
				}

				file->_file_buffer = reinterpret_cast<char*>(GetAllocator()->alloc(file_info.uncompressed_size));
				file->_file_size = file_info.uncompressed_size;

				if (!file->_file_buffer) {
					// Log error
					GetAllocator()->freeT(file);
					return nullptr;
				}

				if (unzReadCurrentFile(*it, file->_file_buffer, static_cast<unsigned int>(file->_file_size)) < 0) {
					// Log error
					GetAllocator()->freeT(file);
					break;
				}

				unzCloseCurrentFile(*it);

				early_out = callback(temp, file);

				if (early_out) {
					break;
				}
			}

			if (early_out) {
				break;
			}

			unzGoToNextFile(*it);
		}
	}

	return early_out;
}

NS_END
