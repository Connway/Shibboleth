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

#include "Shibboleth_OtterUIFileSystem.h"
#include <Gaff_Map.h>

NS_SHIBBOLETH

static Gaff::Map<Otter::AccessFlag, Gaff::File::OPEN_MODE, ProxyAllocator> gOpen_map;
//static bool gMap_init = false;

OtterUIFileSystem::OtterUIFileSystem(void)
{
	//if (!gMap_init) {
	//	gMap_init = true;
	//}

	gOpen_map.setAllocator(ProxyAllocator());
	gOpen_map[Otter::AccessFlag(Otter::kBinary | Otter::kWrite)] = Gaff::File::WRITE_BINARY;
	gOpen_map[Otter::AccessFlag(Otter::kBinary | Otter::kRead)] = Gaff::File::READ_BINARY;
}

OtterUIFileSystem::~OtterUIFileSystem(void)
{
}

void* OtterUIFileSystem::Open(const char* szFilename, Otter::AccessFlag flags)
{
	Gaff::File file(szFilename, gOpen_map[flags]);

	if (!file.isOpen()) {
		return nullptr;
	}

	void* file_ptr = file.getFile();
	_file_map[file_ptr] = Move(file);
	return file_ptr;
}

void OtterUIFileSystem::Close(void* pHandle)
{
	assert(_file_map.indexOf(pHandle) != -1);
	_file_map[pHandle].close();
	_file_map.erase(pHandle);
}

uint32 OtterUIFileSystem::Read(void* pHandle, uint8* data, uint32 count)
{
	assert(_file_map.indexOf(pHandle) != -1);
	return (uint32)_file_map[pHandle].read(data, sizeof(uint8), count);
}

uint32 OtterUIFileSystem::Write(void* pHandle, uint8* data, uint32 count)
{
	assert(_file_map.indexOf(pHandle) != -1);
	return (uint32)_file_map[pHandle].write(data, sizeof(uint8), count);
}

void OtterUIFileSystem::Seek(void* pHandle, uint32 offset, Otter::SeekFlag seekFlag)
{
	assert(_file_map.indexOf(pHandle) != -1);
	_file_map[pHandle].seek(offset, (Gaff::File::SEEK_ORIGIN)(seekFlag >> 1));
}

uint32 OtterUIFileSystem::Size(void* pHandle)
{
	assert(_file_map.indexOf(pHandle) != -1);
	return (uint32)_file_map[pHandle].getFileSize();
}

NS_END
