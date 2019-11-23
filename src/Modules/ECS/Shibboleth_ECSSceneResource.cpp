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
#include <Shibboleth_ResourceAttributesCommon.h>
#include <Shibboleth_LoadFileCallbackAttribute.h>
#include <Shibboleth_SerializeReaderWrapper.h>
#include <Shibboleth_ResourceManager.h>
#include <Shibboleth_LogManager.h>
#include <Shibboleth_Utilities.h>
#include <Gaff_Function.h>

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
	ResourceManager& res_mgr = GetApp().getManagerTFast<ResourceManager>();

	reader.forEachInArray([&](int32_t) -> bool
	{
		char name[256];
		char path[256];
		bool delay_load = false;

		{
			const auto guard = reader.enterElementGuard("name");
			reader.readString(name, ARRAY_SIZE(name));
		}

		{
			const auto guard = reader.enterElementGuard("layer_file");
			reader.readString(path, ARRAY_SIZE(path));
		}

		{
			const auto guard = reader.enterElementGuard("delay_load");
			delay_load = reader.readBool(false);
		}

		_layers.emplace_back(LayerData{
			res_mgr.requestResourceT<ECSLayerResource>(path, delay_load),
			HashString64(name),
		});

		return false;
	});

	const auto callback = Gaff::MemberFunc(this, &ECSSceneResource::layerLoaded);
	Vector<IResource*> resources;

	for (LayerData& layer_data : _layers) {
		if (layer_data.layer->getState() != IResource::RS_DELAYED) {
			resources.emplace_back(layer_data.layer.get());
		}
	}

	res_mgr.registerCallback(resources, callback);
}

void ECSSceneResource::save(Gaff::ISerializeWriter& writer)
{
	GAFF_REF(writer);
}

void ECSSceneResource::layerLoaded(const Vector<IResource*>&)
{
	// Even if we have a failed layer, consider us loaded.
	succeeded();
}

void ECSSceneResource::loadScene(IFile* file)
{
	SerializeReaderWrapper readerWrapper;

	if (!OpenJSONOrMPackFile(readerWrapper, getFilePath().getBuffer(), file)) {
		LogErrorResource("Failed to load scene '%s' with error: '%s'", getFilePath().getBuffer(), readerWrapper.getErrorText());
		failed();
		return;
	}

	load(*readerWrapper.getReader());
}

NS_END

