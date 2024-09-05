/************************************************************************************
Copyright (C) 2024 by Nicholas LaCroix

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

#include "Shibboleth_SceneResource.h"
#include "Shibboleth_SceneLogging.h"
#include <Shibboleth_ResourceAttributesCommon.h>

SHIB_REFLECTION_DEFINE_BEGIN(Shibboleth::SceneResource)
	.classAttrs(
		Shibboleth::ResourceExtensionAttribute(u8".scene.bin"),
		Shibboleth::ResourceExtensionAttribute(u8".scene")
	)

	.template base<Shibboleth::IResource>()
	.template ctor<>()

	.var("deferred_layers", &Shibboleth::SceneResource::_deferred_layers)
	.var("layers", &Shibboleth::SceneResource::_layers)
SHIB_REFLECTION_DEFINE_END(Shibboleth::SceneResource)

namespace
{
	static constexpr auto k_comparison = [](const auto& rhs, const auto& lhs) -> bool { return lhs < rhs; };
}

NS_SHIBBOLETH

SHIB_REFLECTION_CLASS_DEFINE(SceneResource)

const HashString64<>& SceneResource::getDeferredLayerName(int32_t index) const
{
	return _deferred_layers.at(index).first;
}

const HashString64<>& SceneResource::getLayerName(int32_t index) const
{
	return _layers.at(index).first;
}

int32_t SceneResource::getNumDeferredLayers(void) const
{
	return static_cast<int32_t>(_deferred_layers.size());
}

int32_t SceneResource::getNumLayers(void) const
{
	return static_cast<int32_t>(_layers.size());
}

const LayerResource* SceneResource::getDeferredLayer(const HashStringView64<>& name) const
{
	return const_cast<SceneResource*>(this)->getDeferredLayer(name);
}

LayerResource* SceneResource::getDeferredLayer(const HashStringView64<>& name)
{
	return getDeferredLayer(name.getHash());
}

const LayerResource* SceneResource::getDeferredLayer(Gaff::Hash64 name) const
{
	return const_cast<SceneResource*>(this)->getDeferredLayer(name);
}

LayerResource* SceneResource::getDeferredLayer(Gaff::Hash64 name)
{
	const auto it = _deferred_layers.find_as(name, k_comparison);
	return (it != _deferred_layers.end()) ? it->second.get() : nullptr;
}

const LayerResource* SceneResource::getDeferredLayer(int32_t index) const
{
	return const_cast<SceneResource*>(this)->getDeferredLayer(index);
}

LayerResource* SceneResource::getDeferredLayer(int32_t index)
{
	GAFF_ASSERT(Gaff::ValidIndex(index, _deferred_layers));
	return _deferred_layers.at(index).second.get();
}

const LayerResource* SceneResource::getLayer(const HashStringView64<>& name) const
{
	return const_cast<SceneResource*>(this)->getLayer(name);
}

LayerResource* SceneResource::getLayer(const HashStringView64<>& name)
{
	return getLayer(name.getHash());
}

const LayerResource* SceneResource::getLayer(Gaff::Hash64 name) const
{
	return const_cast<SceneResource*>(this)->getLayer(name);
}

LayerResource* SceneResource::getLayer(Gaff::Hash64 name)
{
	const auto it = _layers.find_as(name, k_comparison);

	if (it != _layers.end()) {
		return it->second.get();
	}

	return getDeferredLayer(name);
}

const LayerResource* SceneResource::getLayer(int32_t index) const
{
	return const_cast<SceneResource*>(this)->getLayer(index);
}

LayerResource* SceneResource::getLayer(int32_t index)
{
	GAFF_ASSERT(Gaff::ValidIndex(index, _layers));
	return _layers.at(index).second.get();
}

LayerResource* SceneResource::loadLayer(const HashStringView64<>& name)
{
	return loadLayer(name.getHash());
}

LayerResource* SceneResource::loadLayer(Gaff::Hash64 name)
{
	LayerResource* layer = getDeferredLayer(name);

	if (!layer) {
		return nullptr;
	}

	if (layer->isLoaded()) {
		LogWarningScene("SceneResource::loadLayer: Layer '%s' is already loaded.", layer->getFilePath().getBuffer());
		return nullptr;
	}

	layer->requestLoad();
	return layer;
}

void SceneResource::unloadLayer(const HashStringView64<>& name)
{
	LayerResource* layer = getDeferredLayer(name);

	if (!layer) {
		return;
	}

	if (layer->isDeferred()) {
		LogWarningScene("SceneResource::unloadLayer: Layer '%s' is already unloaded.", layer->getFilePath().getBuffer());
		return;
	}

	layer->requestUnload();
}

NS_END
