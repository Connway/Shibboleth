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

#include "Shibboleth_PrefabResource.h"
#include "Shibboleth_ObjectManager.h"
#include <Shibboleth_ResourceExtensionAttribute.h>
#include <Shibboleth_LoadFileCallbackAttribute.h>
#include <Shibboleth_IFileSystem.h>
#include <Gaff_SerializeReader.h>
#include <Gaff_MessagePack.h>
#include <Gaff_JSON.h>
#include <Gaff_File.h>

SHIB_REFLECTION_DEFINE(PrefabResource)

NS_SHIBBOLETH

SHIB_REFLECTION_CLASS_DEFINE_BEGIN(PrefabResource)
	.classAttrs(ResExtAttribute(".prefab"), ResExtAttribute(".prefab.bin"), MakeLoadFileCallbackAttribute(&PrefabResource::loadPrefab))
	.BASE(IResource)
	.ctor<>()
SHIB_REFLECTION_CLASS_DEFINE_END(PrefabResource)

PrefabResource::~PrefabResource(void)
{
	if (_prefab) {
		GetApp().getManagerTFast<ObjectManager>().destroyObject(_prefab);
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
		// $TODO: Log error
		return false;
	}

	auto reader = Gaff::MakeSerializeReader(json, allocator);
	return _prefab->load(reader);
}

bool PrefabResource::loadMPack(IFile* file, const ProxyAllocator& allocator)
{
	Gaff::MessagePackReader mpack;

	if (!mpack.parse(file->getBuffer(), file->size())) {
		// $TODO: Log error
		return false;
	}

	auto reader = Gaff::MakeSerializeReader(mpack.getRoot(), allocator);
	return _prefab->load(reader);
}

void PrefabResource::loadPrefab(IFile* file)
{
	ObjectManager& obj_mgr = GetApp().getManagerTFast<ObjectManager>();
	ProxyAllocator allocator("Resource");

	_prefab = obj_mgr.createObject();

	if (Gaff::File::CheckExtension(getFilePath().getBuffer(), ".bin")) {
		if (loadMPack(file, allocator)) {
			succeeded();

		} else {
			obj_mgr.destroyObject(_prefab);
			_prefab = nullptr;
			failed();
		}

	} else {
		if (loadJSON(file, allocator)) {
			succeeded();

		} else {
			obj_mgr.destroyObject(_prefab);
			_prefab = nullptr;
			failed();
		}
	}
}

NS_END
