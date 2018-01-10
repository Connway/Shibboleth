/************************************************************************************
Copyright (C) 2017 by Nicholas LaCroix

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

#include "Shibboleth_PrefabResource.h"
#include <Shibboleth_ResourceExtensionAttribute.h>
#include <Shibboleth_IFileSystem.h>
#include <Gaff_SerializeReader.h>
#include <Gaff_MessagePack.h>
#include <Gaff_JSON.h>
#include <Gaff_File.h>

SHIB_REFLECTION_DEFINE(PrefabResource)

NS_SHIBBOLETH

SHIB_REFLECTION_CLASS_DEFINE_BEGIN(PrefabResource)
	.classAttrs(ResExtAttribute(".prefab"), ResExtAttribute(".prefab.bin"))
	.BASE(IResource)
	.ctor<>()
SHIB_REFLECTION_CLASS_DEFINE_END(PrefabResource)

PrefabResource::~PrefabResource(void)
{
	if (_prefab) {
		SHIB_FREET(_prefab, *GetAllocator());
	}
}

void PrefabResource::load(void)
{
	_prefab_file = loadFile(getFilePath().getBuffer());

	if (_prefab_file) {
		Gaff::JobData job_data = { LoadPrefab, this };
		GetApp().getJobPool().addJobs(&job_data, 1, nullptr);
	}
}

const Object* PrefabResource::getPrefab(void) const
{
	return _prefab;
}

bool PrefabResource::loadJSON(IFile* file, const ProxyAllocator& allocator)
{
	Gaff::JSON json;
	
	if (!json.parse(file->getBuffer())) {
		// TODO: Log error
		return false;
	}

	auto reader = Gaff::MakeSerializeReader(json, allocator);
	return _prefab->load(reader);
}

bool PrefabResource::loadMPack(IFile* file, const ProxyAllocator& allocator)
{
	Gaff::MessagePackReader mpack;

	if (!mpack.parse(file->getBuffer(), file->size())) {
		// TODO: Log error
		return false;
	}

	auto reader = Gaff::MakeSerializeReader(mpack.getRoot(), allocator);
	return _prefab->load(reader);
}

void PrefabResource::LoadPrefab(void* data)
{
	PrefabResource* const prefab = reinterpret_cast<PrefabResource*>(data);
	ProxyAllocator allocator("Resource");

	prefab->_prefab = SHIB_ALLOCT(Object, allocator, -1);

	if (Gaff::File::CheckExtension(prefab->getFilePath().getBuffer(), ".bin")) {
		if (prefab->loadMPack(prefab->_prefab_file, allocator)) {
			prefab->_state = RS_LOADED;

		} else {
			SHIB_FREET(prefab->_prefab, allocator);
			prefab->_state = RS_FAILED;
			prefab->_prefab = nullptr;
		}

	} else {
		if (prefab->loadJSON(prefab->_prefab_file, allocator)) {
			prefab->_state = RS_LOADED;

		} else {
			SHIB_FREET(prefab->_prefab, allocator);
			prefab->_state = RS_FAILED;
			prefab->_prefab = nullptr;
		}
	}

	GetApp().getFileSystem()->closeFile(prefab->_prefab_file);
	prefab->_prefab_file = nullptr;

	prefab->callCallbacks();
}

NS_END
