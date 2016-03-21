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

#include "Shibboleth_HoldingLoader.h"
#include "Shibboleth_ModelAnimResources.h"
#include <Shibboleth_IFileSystem.h>
#include <Shibboleth_Utilities.h>
#include <Shibboleth_IApp.h>
#include <Gaff_ScopedExit.h>
#include <Gleam_IModel.h>

NS_SHIBBOLETH

HoldingLoader::HoldingLoader(void)
{
}

HoldingLoader::~HoldingLoader(void)
{
}

Gaff::IVirtualDestructor* HoldingLoader::load(const char* file_name, uint64_t user_data, HashMap<AString, IFile*>& file_map)
{
	AString fname(file_name);
	GAFF_ASSERT(file_map.hasElementWithKey(fname));

	IFile* file = file_map[fname];

	GAFF_SCOPE_EXIT([&]()
	{
		GetApp().getFileSystem()->closeFile(file);
		file_map[fname] = nullptr;
	});

	HoldingData* data = SHIB_ALLOCT(HoldingData, *GetAllocator());

	if (!data) {
		return nullptr;
	}

	size_t ext_index = fname.findLastOf('.');
	ext_index = (ext_index == SIZE_T_FAIL) ? 0 : ext_index;

	data->scene = data->importer.parseMemory(file->getBuffer(), file->size(), static_cast<unsigned int>(user_data), fname.getBuffer() + ext_index);

	if (!data->scene) {
		SHIB_FREET(data, *GetAllocator());
		data = nullptr;

	} //else if (data->scene.hasWarnings()) {
		// Log warnings if they come up
	//}

	return data;
}

NS_END
