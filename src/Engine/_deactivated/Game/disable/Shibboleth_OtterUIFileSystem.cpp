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

#include "Shibboleth_OtterUIFileSystem.h"
#include <Gaff_Map.h>

NS_SHIBBOLETH

OtterUIFileSystem::OtterUIFileSystem(Shibboleth::IFileSystem& file_system):
	_file_system(file_system)
{
}

OtterUIFileSystem::~OtterUIFileSystem(void)
{
	for (auto it = _files.begin(); it != _files.end(); ++it) {
		_file_system.closeFile(*it);
	}
}

void* OtterUIFileSystem::Open(const char* szFilename, Otter::AccessFlag flags)
{
	// To my knowledge, OtterUI doesn't ever request to write any files.
	GAFF_ASSERT(flags == (Otter::kBinary | Otter::kRead));

	IFile* file = _file_system.openFile(szFilename);

	if (!file) {
		// log error
		return nullptr;
	}

	_files.push(file);
	return file;
}

void OtterUIFileSystem::Close(void* pHandle)
{
	auto it = _files.linearSearch(pHandle);
	GAFF_ASSERT(it != _files.end());

	_file_system.closeFile(*it);
	_files.fastErase(it);
}

uint32 OtterUIFileSystem::Read(void* pHandle, uint8* data, uint32 count)
{
	GAFF_ASSERT(_files.linearSearch(pHandle) != _files.end());
	IFile* file = reinterpret_cast<IFile*>(pHandle);
	unsigned int size = Gaff::Min(static_cast<uint32>(file->size()), count);

	memcpy(data, file->getBuffer(), size);
	return size;
}

uint32 OtterUIFileSystem::Write(void* pHandle, uint8* /*data*/, uint32 /*count*/)
{
	GAFF_ASSERT(_files.linearSearch(pHandle) != _files.end());
	return 0;
}

void OtterUIFileSystem::Seek(void* pHandle, uint32 /*offset*/, Otter::SeekFlag /*seekFlag*/)
{
	GAFF_ASSERT(_files.linearSearch(pHandle) != _files.end());
}

uint32 OtterUIFileSystem::Size(void* pHandle)
{
	GAFF_ASSERT(_files.linearSearch(pHandle) != _files.end());
	return static_cast<uint32>(reinterpret_cast<IFile*>(pHandle)->size());
}

NS_END