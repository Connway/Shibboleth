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

#pragma once

#include "Shibboleth_ImageLoader.h"
#include "Shibboleth_ResourceDefines.h"
#include <Shibboleth_IFileSystem.h>
#include <Shibboleth_Utilities.h>
#include <Shibboleth_IApp.h>
#include <Gaff_Image.h>

NS_SHIBBOLETH

ImageLoader::ImageLoader(void)
{
}

ImageLoader::~ImageLoader(void)
{
}

ResourceLoadData ImageLoader::load(const IFile* file, ResourceContainer* res_cont)
{
	SingleDataWrapper<Gaff::Image>* image_data = SHIB_ALLOCT(SingleDataWrapper<Gaff::Image>, *GetAllocator());
	const char* file_name = res_cont->getResourceKey().getBuffer();

	if (!image_data) {
		GetApp().getLogManager().logMessage(LogManager::LOG_ERROR, GetApp().getLogFileName(), "ERROR - Failed to allocate memory for image data.\n");
		return { nullptr };
	}

	if (!image_data->data.init()) {
		GetApp().getLogManager().logMessage(LogManager::LOG_ERROR, GetApp().getLogFileName(), "ERROR - Failed to initialize image data structure when loading '%s'.\n", file_name);
		SHIB_FREET(image_data, *GetAllocator());
		return { nullptr };
	}

	if (!image_data->data.load(file->getBuffer(), static_cast<unsigned int>(file->size()))) {
		GetApp().getLogManager().logMessage(LogManager::LOG_ERROR, GetApp().getLogFileName(), "ERROR - Failed to load image '%s' while processing '%s'.\n", file_name);
		SHIB_FREET(image_data, *GetAllocator());
		return { nullptr };
	}

	return { image_data };
}

NS_END
