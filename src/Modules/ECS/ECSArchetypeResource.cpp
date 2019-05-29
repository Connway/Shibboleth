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

#pragma once

#include <Shibboleth_ECSArchetypeResource.h>
#include <Shibboleth_ResourceExtensionAttribute.h>
#include <Shibboleth_LoadFileCallbackAttribute.h>
#include <Shibboleth_SerializeReaderWrapper.h>
#include <Shibboleth_ECSManager.h>
#include <Shibboleth_LogManager.h>

SHIB_REFLECTION_DEFINE(ECSArchetypeResource)

NS_SHIBBOLETH

SHIB_REFLECTION_CLASS_DEFINE_BEGIN(ECSArchetypeResource)
	.classAttrs(
		ResExtAttribute(".archetype.bin"),
		ResExtAttribute(".archetype"),
		MakeLoadFileCallbackAttribute(&ECSArchetypeResource::loadArchetype)
	)

	.BASE(IResource)
	.ctor<>()
SHIB_REFLECTION_CLASS_DEFINE_END(ECSArchetypeResource)

ECSArchetypeResource::ECSArchetypeResource(void)
{
}

ECSArchetypeResource::~ECSArchetypeResource(void)
{
}

void ECSArchetypeResource::loadArchetype(IFile* file)
{
	SerializeReaderWrapper readerWrapper;

	if (!OpenJSONOrMPackFile(readerWrapper, getFilePath().getBuffer(), file)) {
		LogErrorResource("Failed to load archetype '%s' with error: '%s'", getFilePath().getBuffer(), readerWrapper.getErrorText());
		failed();
	}

	ECSArchetype archetype;

	if (!archetype.finalize(*readerWrapper.getReader())) {
		LogErrorResource("Failed to load archetype '%s'.", getFilePath().getBuffer());
		failed();
		return;
	}

	GetApp().getManagerTFast<ECSManager>().addArchetype(std::move(archetype), _archetype_ref);

	succeeded();
}

NS_END
