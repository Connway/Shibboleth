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

#include <Shibboleth_IFileSystem.h>
#include <Interfaces/IFileSystem.h>
#include <Shibboleth_Array.h>

NS_SHIBBOLETH

class OtterUIFileSystem : public Otter::IFileSystem
{
public:
	OtterUIFileSystem(Shibboleth::IFileSystem& file_system);
	~OtterUIFileSystem(void);

	void* Open(const char* szFilename, Otter::AccessFlag flags);
	void Close(void* pHandle);

	uint32 Read(void* pHandle, uint8* data, uint32 count);
	uint32 Write(void* pHandle, uint8* data, uint32 count);

	void Seek(void* pHandle, uint32 offset, Otter::SeekFlag seekFlag);

	uint32 Size(void* pHandle);

private:
	Shibboleth::IFileSystem& _file_system;
	Array<IFile*> _files;
};

NS_END
