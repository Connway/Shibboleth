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

#include "Shibboleth_Scene.h"
#include "Shibboleth_SceneResource.h"

SHIB_REFLECTION_DEFINE_BEGIN(Shibboleth::Scene)
	.template ctor<Shibboleth::SceneResource&>()

	.func(u8"start", &Shibboleth::Scene::start)
	.func(u8"end", &Shibboleth::Scene::end)
SHIB_REFLECTION_DEFINE_END(Shibboleth::Scene)


NS_SHIBBOLETH

Scene::Scene(SceneResource& scene_resource):
	_scene_resource(scene_resource)
{
	init();
}

void Scene::start(void)
{
}

void Scene::end(void)
{
}

void Scene::init(void)
{
	for (int32_t i = 0; i < _scene_resource.getNumLayers(); ++i) {
		const LayerResource* const layer = _scene_resource.getLayer(i);

		if (!layer) {
			continue;
		}
	}
}

NS_END
