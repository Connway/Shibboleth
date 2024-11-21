/************************************************************************************
Copyright (C) by Nicholas LaCroix

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

#pragma once

#include "Shibboleth_SceneResource.h"
#include "Shibboleth_Scene.h"
#include <Shibboleth_IManager.h>

NS_SHIBBOLETH

class ResourceManager;

class SceneManager final : public IManager
{
public:
	bool initAllModulesLoaded(void) override;
	bool init(void) override;

	void changePrimaryScene(const ResourcePtr<SceneResource>& scene);

	// $TODO: Create scene API
	// $TODO: Request additional scene resources to be loaded.

private:
	Vector<Scene> _scenes{ SCENE_ALLOCATOR };

	ResourcePtr<SceneResource> _primary_scene_resource;
	Scene _primary_scene; // $TODO: Add this to _scenes instead? _scenes[0] is always primary scene?

	ResourceManager* _res_mgr = nullptr;

	void primarySceneLoaded(const Vector<const IResource*>&);
	void loadingScreenFadedIn(void);

	SHIB_REFLECTION_CLASS_DECLARE(SceneManager);
};

NS_END

SHIB_REFLECTION_DECLARE(Shibboleth::SceneManager)
