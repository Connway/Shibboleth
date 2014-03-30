/************************************************************************************
Copyright (C) 2013 by Nicholas LaCroix

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

#include "Gaff_SceneImporter.h"

NS_GAFF

SceneImporter::SceneImporter(void)
{
}

SceneImporter::~SceneImporter(void)
{
}

Scene SceneImporter::loadFile(const char* file_name, unsigned int processing_flags)
{
	return Scene(_importer.ReadFile(file_name, processing_flags));
}

Scene SceneImporter::parseMemory(const void* file, size_t file_len, unsigned int processing_flags, const char* extension_hint)
{
	return Scene(_importer.ReadFileFromMemory(file, file_len, processing_flags, extension_hint));
}

void SceneImporter::destroyScene(void)
{
	_importer.FreeScene();
}

Scene SceneImporter::getScene(void) const
{
	return Scene(_importer.GetScene());
}

Scene SceneImporter::applyProcessing(unsigned int processing_flags)
{
	return Scene(_importer.ApplyPostProcessing(processing_flags));
}

const char* SceneImporter::getError(void) const
{
	return _importer.GetErrorString();
}

size_t SceneImporter::getNumImporters(void) const
{
	return _importer.GetImporterCount();
}

void SceneImporter::setExtraVerboseMode(bool enable)
{
	_importer.SetExtraVerbose(enable);
}

NS_END
