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

#pragma once

//#include <Shibboleth_IResourceManager.h>
#include <Shibboleth_Reflection.h>
#include <Shibboleth_IMainLoop.h>

NS_SHIBBOLETH

//class IRenderManager;
//class IUpdateManager;
//class Object;

class MainLoop : public IMainLoop
{
public:
	bool init(void) override;
	void destroy(void) override;
	void update(void) override;

private:
	//IRenderManager* _render_mgr = nullptr;
	//IUpdateManager* _update_mgr = nullptr;
	//Object* _object = nullptr;
	//Object* _object2 = nullptr;
	//Object* _camera = nullptr;
	//Object* _floor = nullptr;
	//Array<ResourcePtr> _resources;
	//std::mutex _res_lock;

	//void ResReq(ResourcePtr& res);

	SHIB_REFLECTION_CLASS_DECLARE(MainLoop);
};

NS_END

SHIB_REFLECTION_DECLARE(MainLoop)
