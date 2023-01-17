/************************************************************************************
Copyright (C) 2023 by Nicholas LaCroix

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

SHIB_REFLECTION_DEFINE_BEGIN(Shibboleth::SceneManager)
	.base<Shibboleth::IManager>()
	.ctor<>()
SHIB_REFLECTION_DEFINE_END(Shibboleth::SceneManager)


NS_SHIBBOLETH

SHIB_REFLECTION_CLASS_DEFINE(SceneManager)

SceneManager::~SceneManager(void)
{
	//_curr_scene = nullptr;
}

bool SceneManager::initAllModulesLoaded(void)
{
	//const Gaff::JSON starting_scene = GetApp().getConfigs().getObject(u8"scene_starting_scene");

	//if (!starting_scene.isNull() && !starting_scene.isString()) {
	//	LogErrorDefault("No starting scene has been set (or is malformed).");
	//	return false;

	//} else if (starting_scene.isString()) {
	//	const char8_t* const scene = starting_scene.getString();
	//	_curr_scene = GetApp().getManagerTFast<ResourceManager>().requestResourceT<ECSSceneResource>(HashStringView64<>(scene, eastl::CharStrlen(scene)));
	//}

	return true;
}

NS_END
