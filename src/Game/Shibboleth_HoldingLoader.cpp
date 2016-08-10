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
#include <Shibboleth_IApp.h>

NS_SHIBBOLETH

HoldingLoader::HoldingLoader(void)
{
}

HoldingLoader::~HoldingLoader(void)
{
}

ResourceLoadData HoldingLoader::load(const IFile* file, ResourceContainer* res_cont)
{
	const char* file_name = res_cont->getResourceKey().getBuffer();
	HoldingData* data = SHIB_ALLOCT(HoldingData, *GetAllocator());

	if (!data) {
		return { nullptr };
	}

	size_t ext_index = Gaff::FindLastOf(file_name, '.');
	ext_index = (ext_index == SIZE_T_FAIL) ? 0 : ext_index;

	data->scene = data->importer.parseMemory(file->getBuffer(), file->size(), static_cast<unsigned int>(res_cont->getUserData()), file_name + ext_index);

	if (!data->scene) {
		SHIB_FREET(data, *GetAllocator());
		data = nullptr;

	} //else if (data->scene.hasWarnings()) {
		// Log warnings if they come up
	//}

	return { data };
}

NS_END
