/************************************************************************************
Copyright (C) 2022 by Nicholas LaCroix

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

#include <Shibboleth_ECSArchetypeResource.h>
#include "Shibboleth_ECSManager.h"
#include <Shibboleth_ResourceAttributesCommon.h>
#include <Shibboleth_LoadFileCallbackAttribute.h>
#include <Shibboleth_SerializeReaderWrapper.h>
#include <Shibboleth_ResourceLogging.h>
#include <Shibboleth_AppUtils.h>

SHIB_REFLECTION_DEFINE_BEGIN(Shibboleth::ECSArchetypeResource)
	.classAttrs(
		Shibboleth::CreatableAttribute(),
		Shibboleth::ResExtAttribute(u8".archetype.bin"),
		Shibboleth::ResExtAttribute(u8".archetype"),
		Shibboleth::MakeLoadFileCallbackAttribute(&Shibboleth::ECSArchetypeResource::loadArchetype)
	)

	.template base<Shibboleth::IResource>()
	.template ctor<>()
SHIB_REFLECTION_DEFINE_END(Shibboleth::ECSArchetypeResource)

NS_SHIBBOLETH

SHIB_REFLECTION_CLASS_DEFINE(ECSArchetypeResource)

ECSArchetypeResource::ECSArchetypeResource(void)
{
}

ECSArchetypeResource::~ECSArchetypeResource(void)
{
}

const ECSArchetype& ECSArchetypeResource::getArchetype(void) const
{
	return _archetype_ref->getArchetype();
}

void ECSArchetypeResource::loadArchetype(IFile* file, uintptr_t /*thread_id_int*/)
{
	SerializeReaderWrapper readerWrapper;

	if (!OpenJSONOrMPackFile(readerWrapper, getFilePath().getBuffer(), file)) {
		LogErrorResource("Failed to load archetype '%s' with error: '%s'", getFilePath().getBuffer(), readerWrapper.getErrorText());
		failed();
		return;
	}

	ECSArchetype archetype;

	if (!archetype.finalize(*readerWrapper.getReader())) {
		LogErrorResource("Failed to load archetype '%s'.", getFilePath().getBuffer());
		failed();
		return;
	}

	GetManagerTFast<ECSManager>().addArchetype(std::move(archetype), _archetype_ref);

	succeeded();
}

NS_END
