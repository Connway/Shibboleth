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

#include "Shibboleth_ECSSceneResource.h"
#include <Shibboleth_ResourceExtensionAttribute.h>
#include <Shibboleth_LoadFileCallbackAttribute.h>
#include <Shibboleth_SerializeReaderWrapper.h>
#include <Shibboleth_LogManager.h>
#include <Shibboleth_Utilities.h>

SHIB_REFLECTION_DEFINE(ECSSceneResource)

NS_SHIBBOLETH

SHIB_REFLECTION_CLASS_DEFINE_BEGIN(ECSSceneResource)
	.classAttrs(
		ResExtAttribute(".scene.bin"),
		ResExtAttribute(".scene"),
		MakeLoadFileCallbackAttribute(&ECSSceneResource::loadScene)
	)

	.BASE(IResource)
	.ctor<>()
SHIB_REFLECTION_CLASS_DEFINE_END(ECSSceneResource)

ECSSceneResource::ECSSceneResource(void)
{
}

ECSSceneResource::~ECSSceneResource(void)
{
}

void ECSSceneResource::load(const Gaff::ISerializeReader& reader)
{
	GAFF_REF(reader);
}

void ECSSceneResource::save(Gaff::ISerializeWriter& writer)
{
	GAFF_REF(writer);
}

void ECSSceneResource::loadScene(IFile* file)
{
	SerializeReaderWrapper readerWrapper;

	if (!OpenJSONOrMPackFile(readerWrapper, getFilePath().getBuffer(), file)) {
		LogErrorResource("Failed to load scene '%s' with error: '%s'", getFilePath().getBuffer(), readerWrapper.getErrorText());
		failed();
	}

	const Gaff::ISerializeReader& reader = *readerWrapper.getReader();

	GAFF_REF(reader);
	//_layer.load(*readerWrapper.getReader());
	succeeded();
}

NS_END

