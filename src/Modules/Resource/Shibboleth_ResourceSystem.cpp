/************************************************************************************
Copyright (C) 2021 by Nicholas LaCroix

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

#include "Shibboleth_ResourceSystem.h"
#include "Shibboleth_ResourceManager.h"

SHIB_REFLECTION_DEFINE_WITH_CTOR_AND_BASE(ResourceSystem, ISystem)

NS_SHIBBOLETH

SHIB_REFLECTION_CLASS_DEFINE(ResourceSystem)

bool ResourceSystem::init(void)
{
	_res_mgr = &GetApp().getManagerTFast<ResourceManager>();
	return true;
}

void ResourceSystem::update(uintptr_t /*thread_id_int*/)
{
	_res_mgr->checkCallbacks();
	_res_mgr->checkAndRemoveResources();
}

NS_END