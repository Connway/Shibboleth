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
#include <Shibboleth_ResourceManager.h>
#include <Gaff_Function.h>

SHIB_REFLECTION_DEFINE_BEGIN(Shibboleth::SceneManager)
	.template base<Shibboleth::IManager>()
	.template ctor<>()

	.var(u8"primary_scene_resource", &Shibboleth::SceneManager::_primary_scene_resource)
SHIB_REFLECTION_DEFINE_END(Shibboleth::SceneManager)

namespace
{
	static Shibboleth::ProxyAllocator s_allocator{ SCENE_ALLOCATOR };
}

NS_SHIBBOLETH

SHIB_REFLECTION_CLASS_DEFINE(SceneManager)

bool SceneManager::initAllModulesLoaded(void)
{
	_res_mgr = &GetManagerTFast<ResourceManager>();

	const SceneConfig& config = GetConfigRef<SceneConfig>();
	changePrimaryScene(config.starting_scene);

	return true;
}

bool SceneManager::init(void)
{
	GetApp().getLogManager().addChannel(HashStringView32<>{ k_log_channel_name_scene });
	return true;
}

void SceneManager::changePrimaryScene(const ResourcePtr<SceneResource>& scene)
{
	GAFF_ASSERT(scene && !scene->hasFailed());

	// $TODO: Tell loading screen to display. Register for callback to call loadingScreenFadedIn().
	loadingScreenFadedIn();

	_primary_scene_resource = scene;

	_res_mgr->registerCallback(*_primary_scene_resource, Gaff::MemberFunc(this, &SceneManager::primarySceneLoaded));
}

void SceneManager::primarySceneLoaded(const Vector<const IResource*>&)
{
	// $TODO: Tell loading screen we are done loading.

	_primary_scene = Scene{};

	_primary_scene.init(*_primary_scene_resource);
	_primary_scene.start();
}

void SceneManager::loadingScreenFadedIn(void)
{
	_primary_scene.end();
}

NS_END
