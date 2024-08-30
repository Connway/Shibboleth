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

#include "Shibboleth_SceneManager.h"
#include "Shibboleth_SceneLogging.h"
#include "Shibboleth_SceneConfig.h"

SHIB_REFLECTION_DEFINE_BEGIN(Shibboleth::SceneManager)
	.template base<Shibboleth::IManager>()
	.template ctor<>()

	.var(u8"primary_scene", &Shibboleth::SceneManager::_primary_scene)
SHIB_REFLECTION_DEFINE_END(Shibboleth::SceneManager)


NS_SHIBBOLETH

SHIB_REFLECTION_CLASS_DEFINE(SceneManager)

bool SceneManager::initAllModulesLoaded(void)
{
	const SceneConfig& config = GetConfigRef<SceneConfig>();
	changePrimaryScene(config.starting_scene);

	return true;
}

bool SceneManager::init(void)
{
	GetApp().getLogManager().addChannel(HashStringView32<>{ k_log_channel_name_scene });
	return true;
}

void SceneManager::changePrimaryScene(const DeferredResourcePtr<SceneResource>& scene)
{
	GAFF_ASSERT(scene && !scene->hasFailed());

	if (_primary_scene) {
		// $TODO: Unload old scene.
	}

	_primary_scene = scene;

	if (_primary_scene->isDeferred()) {
		// $TODO: Display loading screen scene.
		_primary_scene->requestLoad();
	}
}

NS_END
