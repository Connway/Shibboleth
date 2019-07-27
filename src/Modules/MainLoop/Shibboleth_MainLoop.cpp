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

#include "Shibboleth_MainLoop.h"
//#include "Shibboleth_SetupFunctions.h"
//#include <Shibboleth_IRenderPipelineManager.h>
//#include <Shibboleth_IOcclusionManager.h>
//#include <Shibboleth_IObjectManager.h>
//#include <Shibboleth_IRenderManager.h>
//#include <Shibboleth_IUpdateManager.h>
//#include <Shibboleth_ISceneManager.h>
#include <Shibboleth_Utilities.h>
#include <Shibboleth_IApp.h>

#include <Shibboleth_RenderManagerBase.h>
#include <Shibboleth_ResourceManager.h>
#include <Shibboleth_ECSSceneResource.h>
#include <Shibboleth_ModelResource.h>
#include <Gleam_IRenderDevice.h>

SHIB_REFLECTION_DEFINE(MainLoop)

NS_SHIBBOLETH

SHIB_REFLECTION_CLASS_DEFINE_BEGIN(MainLoop)
	.BASE(IMainLoop)
	.ctor<>()
SHIB_REFLECTION_CLASS_DEFINE_END(MainLoop)

//static size_t g_image_pool_index = 0;
//
//void* MEMCB ImageAlloc(const size_t size)
//{
//	return ShibbolethAllocate(size, g_image_pool_index);
//}
//
//void MEMCB ImageFree(const void* const data)
//{
//	ShibbolethFree(const_cast<void*>(data));
//}


bool MainLoop::init(void)
{
	return true;
}

void MainLoop::destroy(void)
{
}

void MainLoop::update(void)
{
	_render_mgr->updateWindows(); // This has to happen in the main thread.
	//_update_mgr->update();

	//std::this_thread::yield();

	ResourceManager& res_mgr = GetApp().getManagerTFast<ResourceManager>();

	auto scene_res = res_mgr.requestResourceT<ECSSceneResource>("Scenes/test.scene");

	res_mgr.waitForResource(*scene_res);

	if (scene_res->hasFailed()) {
		GetApp().quit();
		return;
	}

	auto model_res = res_mgr.requestResourceT<ModelResource>("Models/ninja.model");

	res_mgr.waitForResource(*model_res);

	if (model_res->hasFailed()) {
		GetApp().quit();
		return;
	}

	GetApp().quit();
}

NS_END
