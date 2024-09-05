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

#include "Shibboleth_Layer.h"
#include "Shibboleth_LayerResource.h"

SHIB_REFLECTION_DEFINE_BEGIN(Shibboleth::Layer)
	.template ctor<>()

	.var(u8"layer_resource", &Shibboleth::Layer::_layer_resource)
	.var(u8"entities", &Shibboleth::Layer::_entities)

	.func(u8"start", &Shibboleth::Layer::start)
	.func(u8"end", &Shibboleth::Layer::end)
SHIB_REFLECTION_DEFINE_END(Shibboleth::Layer)


NS_SHIBBOLETH

void Layer::init(const DeferredResourcePtr<LayerResource>& layer_resource)
{
	_layer_resource = layer_resource;
}

void Layer::init(LayerResource& layer_resource)
{
	_entities = std::move(layer_resource.releaseEntityDefinitions());

	for (Entity& entity : _entities) {
		entity.init();
	}
}

void Layer::setName(const HashStringView64<>& name)
{
	_name = name;
}

const HashString64<>& Layer::getName(void) const
{
	return _name;
}

bool Layer::hasRequestedLoad(void) const
{
	return _flags.testAll(Flag::RequestedLoad);
}

bool Layer::isDeferred(void) const
{
	return _layer_resource;
}

void Layer::unload(void)
{
	GAFF_ASSERT(hasRequestedLoad());
	_layer_resource->requestUnload();
}

void Layer::load(void)
{
	GAFF_ASSERT(!hasRequestedLoad());
	_layer_resource->requestLoad();

	// $TODO: Set callback for when resource is finished loading.
}

void Layer::start(void)
{
	for (Entity& entity : _entities) {
		entity.start();
	}
}

void Layer::end(void)
{
	for (Entity& entity : _entities) {
		entity.end();
	}
}

NS_END
