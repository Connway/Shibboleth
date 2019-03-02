/************************************************************************************
Copyright (C) 2019 by Nicholas LaCroix

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

#include "Shibboleth_Utilities.h"
#include "Shibboleth_SerializeReaderWrapper.h"
#include <Shibboleth_IFileSystem.h>
#include <Shibboleth_Memory.h>
#include <Shibboleth_IApp.h>
#include <Gaff_Utils.h>

NS_SHIBBOLETH

static IApp* gApp = nullptr;

void SetApp(IApp& app)
{
	gApp = &app;
}

IApp& GetApp(void)
{
	GAFF_ASSERT(gApp);
	return *gApp;
}

bool OpenJSONOrMPackFile(SerializeReaderWrapper& wrapper, const char* path)
{
	IApp& app = GetApp();
	IFileSystem& fs = app.getFileSystem();

	U8String bin_path = U8String(path) + ".bin";

	IFile* file = fs.openFile(bin_path.c_str());

	if (file) {
		return wrapper.parseMPack(file->getBuffer(), file->size());

	} else {
		file = fs.openFile(path);

		if (file) {
			return wrapper.parseJSON(file->getBuffer());
		}
	}

	return false;
}

NS_END

GAFF_STATIC_FILE_FUNC
{
	Gaff::JSON::SetMemoryFunctions(&Shibboleth::ShibbolethAllocate, &Shibboleth::ShibbolethFree);
}
