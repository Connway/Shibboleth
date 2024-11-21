/************************************************************************************
Copyright (C) by Nicholas LaCroix

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

#include "FileSystem/Shibboleth_LooseFileSystem.h"
#include "Shibboleth_String.h"
#include <Gaff_ContainerAlgorithm.h>
#include <Gaff_Utils.h>
#include <Gaff_File.h>
#include <filesystem>

SHIB_REFLECTION_DEFINE_BEGIN(Shibboleth::LooseFileSystem)
	.template base<Shibboleth::IFileSystem>()
	.template ctor<>()
SHIB_REFLECTION_DEFINE_END(Shibboleth::LooseFileSystem)

NS_SHIBBOLETH

SHIB_REFLECTION_CLASS_DEFINE(LooseFileSystem)

LooseFile::LooseFile(void)
{
}

LooseFile::~LooseFile(void)
{
	if (_file_buffer) {
		SHIB_FREE(_file_buffer, GetAllocator());
	}
}

size_t LooseFile::size(void) const
{
	return _file_size;
}

const int8_t* LooseFile::getBuffer(void) const
{
	return _file_buffer;
}

int8_t* LooseFile::getBuffer(void)
{
	return _file_buffer;
}


LooseFileSystem::LooseFileSystem(void)
{
}

LooseFileSystem::~LooseFileSystem(void)
{
	for (auto it = _files.begin(); it != _files.end(); ++it) {
		SHIB_FREET(it->file, GetAllocator());
	}
}

IFile* LooseFileSystem::openFile(const char8_t* file_name)
{
	GAFF_ASSERT(file_name && eastl::CharStrlen(file_name));
	const EA::Thread::AutoMutex lock(_file_lock);

	auto it = Gaff::Find(_files, file_name,
	[](const FileData& lhs, const char8_t* rhs) -> bool
	{
		return lhs.name == rhs;
	});

	if (it != _files.end()) {
		++it->count;
		return it->file;
	}

	const U8String name = U8String(u8"./") + file_name; // Pre-pend './' to name
	Gaff::File loose_file;

	if (!loose_file.open(name.data(), Gaff::File::OpenMode::ReadBinary)) {
		return nullptr;
	}

	LooseFile* const file = SHIB_ALLOCT(LooseFile, GetAllocator());

	// Should probably log that the allocation failed
	if (!file) {
		return nullptr;
	}

	file->_file_size= loose_file.getFileSize();
	file->_file_buffer = SHIB_ALLOC_CAST(int8_t*, file->_file_size + 1, GetAllocator());

	if (!file->_file_buffer) {
		SHIB_FREET(it->file, GetAllocator());
		return nullptr;
	}

	if (!loose_file.readEntireFile(reinterpret_cast<char*>(file->_file_buffer))) {
		SHIB_FREET(it->file, GetAllocator());
		return nullptr;
	}

	file->_file_buffer[file->_file_size] = 0;

	FileData file_data;
	file_data.name = file_name;
	file_data.file = file;
	file_data.count = 1;

	_files.emplace_back(std::move(file_data));
	return file;
}

void LooseFileSystem::closeFile(const IFile* file)
{
	GAFF_ASSERT(file);
	const EA::Thread::AutoMutex lock(_file_lock);

	auto it = Gaff::Find(_files, file,
	[](const FileData& lhs, const IFile* rhs) -> bool
	{
		return lhs.file == rhs;
	});

	if (it != _files.end()) {
		int32_t new_count = --it->count;

		if (!new_count) {
			SHIB_FREET(it->file, GetAllocator());
			_files.erase_unsorted(it);
		}
	}
}

bool LooseFileSystem::forEachFile(const char8_t* directory, eastl::function<bool(const char8_t*, IFile*)>& callback, const char8_t* extension, bool recursive)
{
	if (!std::filesystem::is_directory(directory)) {
		return false;
	}

	for (const auto& dir_entry : std::filesystem::directory_iterator(directory)) {
		if (!recursive && !dir_entry.is_regular_file()) {
			continue;
		}

		const auto* file_name = dir_entry.path().c_str();
		CONVERT_STRING(char8_t, temp, file_name);

		if (recursive && dir_entry.is_directory()) {
			forEachFile(temp, callback, extension, recursive);
			continue;
		}

		if (!dir_entry.is_regular_file()) {
			continue;
		}

		if (extension && !Gaff::EndsWith(temp, extension)) {
			continue;
		}

		IFile* const file = openFile(temp);

		//if (!file) {
		//	// $TODO: Log error
		//	continue;
		//}

		if (callback(temp, file)) {
			return true;
		}
	}

	return false;
}

bool LooseFileSystem::forEachFile(const char8_t* directory, eastl::function<bool (const char8_t*, IFile*)>& callback, bool recursive)
{
	return forEachFile(directory, callback, nullptr, recursive);
}

NS_END
