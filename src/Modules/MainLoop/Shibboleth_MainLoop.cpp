/************************************************************************************
Copyright (C) 2018 by Nicholas LaCroix

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
//#include <Shibboleth_Object.h>
#include <Shibboleth_IApp.h>
//#include <Gaff_Image.h>

//#include <Shibboleth_ICameraComponent.h>

//#include <Shibboleth_ResourceManager.h>
//#include <Shibboleth_PrefabResource.h>

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
	//_render_mgr = &GetApp().getManagerT<IRenderManager>();
	//_update_mgr = &GetApp().getManagerT<IUpdateManager>();

	//g_image_pool_index = GetPoolIndex("Images");
	//Gaff::Image::SetMemoryFunctions(ImageAlloc, ImageFree);
	//Gaff::Image::SysInit();

	//if (!LoadGraphicsModule()) {
	//	return false;
	//}

	//if (!CreateResourceLoaders()) {
	//	return false;
	//}

	//if (!SetupDevices()) {
	//	return false;
	//}

	//// Block until loaded.
	//IApp& app = GetApp();
	//app.getManagerT<IResourceManager>().addRequestAddedCallback(Gaff::Bind(this, &MainLoop::ResReq));
	//app.getManagerT<ISceneManager>().loadScene("Resources/Scenes/test.scene");

	//bool all_loaded = true;

	//do {
	//	all_loaded = true;

	//	std::lock_guard<std::mutex> lock(_res_lock);

	//	for (size_t i = 0; i < _resources.size(); ++i) {
	//		all_loaded &= _resources[i]->isLoaded();

	//		if (_resources[i]->hasFailed()) {
	//			all_loaded = true;
	//			app.quit();
	//			return false;
	//		}
	//	}
	//} while (!all_loaded || _resources.empty());

	//// Set the camera
	//Object* camera = app.getManagerT<IObjectManager>().findObject("Test Camera");

	//if (camera) {
	//	ICameraComponent* camera_cmp = camera->getFirstComponentWithInterface<ICameraComponent>();

	//	if (camera_cmp) {
	//		app.getManagerT<IRenderPipelineManager>().setOutputCamera(camera_cmp);
	//	}
	//}

	return true;
}

void MainLoop::destroy(void)
{
	//IApp& app = GetApp();
	//auto& occ_mgr = GetApp().getManagerT<IOcclusionManager>();
	//auto& obj_mgr = GetApp().getManagerT<IObjectManager>();

	//if (_object) {
	//	occ_mgr.removeObject(_object);
	//	obj_mgr.removeObject(_object->getID());
	//}

	//if (_object2) {
	//	occ_mgr.removeObject(_object2);
	//	obj_mgr.removeObject(_object2->getID());
	//}

	//if (_camera) {
	//	obj_mgr.removeObject(_camera->getID());
	//}

	//if (_floor) {
	//	obj_mgr.removeObject(_floor->getID());
	//}

	//app.getManagerT<IResourceManager>().removeRequestAddedCallback(Gaff::Bind(this, &MainLoop::ResReq));
	//_resources.clear();

	//Gaff::Image::SysShutdown();
}

void MainLoop::update(void)
{
	//_render_mgr->updateWindows(); // This has to happen in the main thread.
	//_update_mgr->update();

	//std::this_thread::yield();

	//ResourceManager& res_mgr = GetApp().getManagerTFast<ResourceManager>();
	//PrefabResourcePtr prefab = res_mgr.requestResourceT<PrefabResource>("Objects/test2.prefab");

	//res_mgr.waitForResource(*prefab);

	//if (prefab->hasFailed()) {
	//	GetApp().quit();
	//	return;
	//}

	//const Object* const object = prefab->getPrefab();

	//const AngelScriptComponent* const asc = object->getComponent<AngelScriptComponent>();
	//const AngelScriptResourcePtr& script = asc->getScript();

	//res_mgr.waitForResource(*script);

	GetApp().quit();
}

//void MainLoop::ResReq(ResourcePtr& res)
//{
//	std::lock_guard<std::mutex> lock(_res_lock);
//	_resources.emplacePush(res);
//}

NS_END
